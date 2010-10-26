#include "emacsinstance.h"

#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <string>
#include <sstream>

#include "message_proxy.h"
#include "process_watcher.h"
#include "scriptobject.h"
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

bool EmacsInstance::startEditor()
{
    if (!window_id_) {
        fprintf(stderr, "ERROR: Window not yet set\n");
        return false;
    }
    if (child_pid_ || !temp_file_.empty()) {
        fprintf(stderr, "ERROR: Child process already launched\n");
        return false;
    }

    // TODO: Make a scoped temporary file or something.
    // FIXME: Do file io on a dedicated thread.
    const char *tmpdir = getenv("TMPDIR");
    if (!tmpdir)
        tmpdir = "/tmp";
    std::string temp_file(tmpdir);
    temp_file += "/.emacs-npapi.XXXXXX";
    int fd = mkstemp(const_cast<char*>(temp_file.c_str()));
    if (fd < 0) {
        perror("mkstemp");
        return false;
    }
    temp_file_ = temp_file;
    FILE* file = fdopen(fd, "w");
    if (!file) {
        perror("fdopen");
        close(fd);
        return false;
    }
    fwrite(initial_text_.c_str(), initial_text_.size(), 1, file);
    fclose(file);

    std::stringstream ss;
    ss << window_id_;
    std::string window_str = ss.str();

    // Apparently g_spawn_async wants non-const. Sigh.
    char **argv = g_new(char*, 5);
    argv[0] = g_strdup("emacs");
    argv[1] = g_strdup("--parent-id");
    argv[2] = g_strdup(window_str.c_str());
    argv[3] = g_strdup(temp_file_.c_str());
    argv[4] = NULL;
    GPid pid;
    if (!g_spawn_async(NULL, argv, NULL,
		       static_cast<GSpawnFlags>(
                           G_SPAWN_DO_NOT_REAP_CHILD | G_SPAWN_SEARCH_PATH),
		       NULL, NULL, &pid, NULL)) {
        g_strfreev(argv);
        fprintf(stderr, "ERROR: Failed to spawn emacs\n");
        return false;
    }
    g_strfreev(argv);
    process_watcher::watchProcess(pid, this);

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
        task->run();
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
