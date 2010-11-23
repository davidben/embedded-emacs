// Copyright (c) 2010 David Benjamin. All rights reserved.
// Use of this source code is governed by an MIT-style license that can be
// found in the LICENSE file.
#include "emacs_instance.h"

#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <string>
#include <sstream>

#include "command_template.h"
#include "message_proxy.h"
#include "process_watcher.h"
#include "script_object.h"
#include "task.h"

namespace {

void deleteTask(void *ptr)
{
    delete static_cast<Task*>(ptr);
}

void processTasksThunk(void *ptr)
{
    static_cast<EmacsInstance*>(ptr)->processTasks();
}

};

EmacsInstance::EmacsInstance(NPP npp)
        : npp_(npp),
          window_id_(0),
          child_pid_(0),
          script_object_(NULL),
          callback_(NULL),
          message_proxy_(NULL)
{
    task_queue_ = g_async_queue_new_full(deleteTask);
}

EmacsInstance::~EmacsInstance()
{
    if (message_proxy_) {
        message_proxy_->invalidate();
        message_proxy_->unref();
        message_proxy_ = NULL;
    }
    setCallback(NULL);
    if (script_object_)
        NPN_ReleaseObject(script_object_);
    if (!temp_file_.empty()) {
        if (unlink(temp_file_.c_str()) < 0)
            perror("unlink");
    }
    g_async_queue_unref(task_queue_);
}

bool EmacsInstance::startEditor(std::string *error)
{
    if (!window_id_) {
        fprintf(stderr, "Error: Window not yet set\n");
        *error = "window not yet set";
        return false;
    }
    if (child_pid_ || !temp_file_.empty()) {
        fprintf(stderr, "Error: Child process already launched\n");
        *error = "child process already launched";
        return false;
    }

    // FIXME: REALLY REALLY need a scoped temporary file.
    // FIXME: Do file io on a dedicated thread.
    const char *tmpdir = getenv("TMPDIR");
    if (!tmpdir)
        tmpdir = "/tmp";
    std::string temp_file(tmpdir);
    temp_file += "/.emacs-npapi.XXXXXX";
    int fd = mkstemp(const_cast<char*>(temp_file.c_str()));
    if (fd < 0) {
        perror("mkstemp");
        *error = "failed to create temporary file";
        return false;
    }
    FILE* file = fdopen(fd, "w");
    if (!file) {
        perror("fdopen");
        close(fd);
        if (unlink(temp_file.c_str()) < 0)
            perror("unlink");
        *error = "failed to fdopen temporary file";
        return false;
    }
    fwrite(initial_text_.c_str(), initial_text_.size(), 1, file);
    fclose(file);

    std::stringstream ss;
    ss << window_id_;
    std::string window_str = ss.str();

    std::vector<std::string> params;
    command_template::Environment env;
    env["WINDOW"] = window_str;
    env["PATH"] = temp_file;
    if (!command_template::applyTemplate(editor_command_, env, params, error)) {
        if (unlink(temp_file.c_str()) < 0)
            perror("unlink");
        return false;
    }
    if (params.size() == 0) {
        *error = "empty parameter list";
        if (unlink(temp_file.c_str()) < 0)
            perror("unlink");
        return false;
    }

    char **argv = command_template::stringVectorToArgv(params);
    GPid pid;
    if (!g_spawn_async(NULL, argv, NULL,
		       static_cast<GSpawnFlags>(
                           G_SPAWN_DO_NOT_REAP_CHILD | G_SPAWN_SEARCH_PATH),
		       NULL, NULL, &pid, NULL)) {
        g_strfreev(argv);
        *error = "failed to spawn child process";
        fprintf(stderr, "Error: Failed to spawn child process\n");
        if (unlink(temp_file.c_str()) < 0)
            perror("unlink");
        return false;
    }
    g_strfreev(argv);
    process_watcher::watchProcess(pid, this);

    temp_file_ = temp_file;
    child_pid_ = pid;
    return true;
}

void EmacsInstance::setCallback(NPObject* callback)
{
    // TODO: Write fancy smart pointer.
    if (callback)
        NPN_RetainObject(callback);
    if (callback_)
        NPN_ReleaseObject(callback_);
    callback_ = callback;
}

void EmacsInstance::setInitialText(const char *utf8Chars, uint32_t len)
{
    initial_text_.assign(utf8Chars, len);
}

void EmacsInstance::setEditorCommand(const char *utf8Chars, uint32_t len)
{
    editor_command_.assign(utf8Chars, len);
}

MessageProxy* EmacsInstance::getMessageProxy()
{
    if (!message_proxy_) {
        message_proxy_ = new MessageProxy(this);
    }
    return message_proxy_;
}

void EmacsInstance::postTask(Task* task)
{
    if (!task) return;
    g_async_queue_push(task_queue_, task);
    // No memory is allocated, so we don't care if this runs or
    // not. It may also run more often than necessary, but that's
    // okay.
    NPN_PluginThreadAsyncCall(npp_, processTasksThunk, this);
}

void EmacsInstance::processTasks()
{
    void* data;
    while ((data = g_async_queue_try_pop(task_queue_))) {
        Task* task = static_cast<Task*>(data);
        task->run(this);
        delete task;
    }
}

void EmacsInstance::childExited(pid_t pid, int status)
{
    if (child_pid_ != pid) {
        fprintf(stderr, "WARNING: Unexpected child exit (pid %d)\n", pid);
        return;
    }
    if (!callback_) {
        fprintf(stderr, "WARNING: No callback defined.\n");
        return;
    }
    if (temp_file_.empty()) {
        fprintf(stderr, "ERROR: No temporary file???\n");
        return;
    }

    NPVariant args[2];
    INT32_TO_NPVARIANT(status, args[1]);

    // Get the file contents.
    gchar *contents;
    gsize length;
    if (g_file_get_contents(temp_file_.c_str(),
                            &contents, &length,
                            NULL)) {
        // Copy to NPN_MemAlloc-compatible buffer.
        NPUTF8 *np_contents = static_cast<NPUTF8*>(NPN_MemAlloc(length));
        memcpy(np_contents, contents, length);
        g_free(contents);
        STRINGN_TO_NPVARIANT(np_contents, length, args[0]);
    } else {
        NULL_TO_NPVARIANT(args[0]);
    }

    NPVariant result;
    NPN_InvokeDefault(npp_, callback_, args, 2, &result);
    NPN_ReleaseVariantValue(&args[0]);
    NPN_ReleaseVariantValue(&args[1]);
    NPN_ReleaseVariantValue(&result);
}

NPError EmacsInstance::setWindow(NPWindow* window)
{
    long window_id = reinterpret_cast<long>(window->window);
    if (!window_id_) {
        window_id_ = window_id;
    } else {
        // This really shouldn't happen.
        // TODO: Notify javascript about it.
        if (window_id != window_id_)
            return NPERR_GENERIC_ERROR;
    }
    return NPERR_NO_ERROR;
}

NPObject* EmacsInstance::getScriptObject()
{
    if (!script_object_) {
        script_object_ = ScriptObject::create(npp_);
        NPN_RetainObject(script_object_);
    }
    return script_object_;
}
