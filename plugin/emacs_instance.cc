// Copyright (c) 2011 David Benjamin. All rights reserved.
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
#include "emacs_manager.h"
#include "npapi-cxx/browser.h"

EmacsInstance::EmacsInstance(EmacsManager* parent,
                             long window_id,
                             const std::string& editor_command,
                             const char *initial_text, uint32_t text_len,
                             NPObject *callback)
        : parent_(parent),
          child_pid_(0) {
    if (startEditor(window_id, editor_command, initial_text, text_len)) {
        callback_.reset(callback);
    }
}

EmacsInstance::~EmacsInstance() {
    if (!temp_file_.empty()) {
        if (unlink(temp_file_.c_str()) < 0)
            perror("unlink");
    }
}

bool EmacsInstance::startEditor(long window_id,
                                const std::string& editor_command,
                                const char *initial_text, uint32_t text_len) {
    if (!window_id) {
        fprintf(stderr, "Error: Invalid window.\n");
        error_ = "invalid window";
        return false;
    }

    // FIXME: REALLY REALLY need a scoped temporary file.
    // FIXME: Do file io on a dedicated thread.
    const char *tmpdir = getenv("TMPDIR");
    if (!tmpdir)
        tmpdir = "/tmp";
    std::string temp_file(tmpdir);
    temp_file += "/.embedded-emacs.XXXXXX";
    int fd = mkstemp(const_cast<char*>(temp_file.c_str()));
    if (fd < 0) {
        perror("mkstemp");
        error_ = "failed to create temporary file";
        return false;
    }
    FILE* file = fdopen(fd, "w");
    if (!file) {
        perror("fdopen");
        close(fd);
        if (unlink(temp_file.c_str()) < 0)
            perror("unlink");
        error_ = "failed to fdopen temporary file";
        return false;
    }
    fwrite(initial_text, text_len, 1, file);
    fclose(file);

    std::stringstream ss;
    ss << window_id;
    std::string window_str = ss.str();

    std::vector<std::string> params;
    command_template::Environment env;
    env["WINDOW"] = window_str;
    env["PATH"] = temp_file;
    if (!command_template::applyTemplate(editor_command, env, params, &error_)) {
        if (unlink(temp_file.c_str()) < 0)
            perror("unlink");
        return false;
    }
    if (params.size() == 0) {
        error_ = "empty parameter list";
        if (unlink(temp_file.c_str()) < 0)
            perror("unlink");
        return false;
    }

    char **argv = command_template::stringVectorToArgv(params);
    GPid pid;
    GError *gerror = NULL;
    if (!g_spawn_async(NULL, argv, NULL,
		       static_cast<GSpawnFlags>(
                           G_SPAWN_DO_NOT_REAP_CHILD | G_SPAWN_SEARCH_PATH),
		       NULL, NULL, &pid, &gerror)) {
        g_strfreev(argv);
        error_ = std::string("failed to spawn child process: ") +
                gerror->message;
        fprintf(stderr, "Error: Failed to spawn child process: %s\n",
                gerror->message);
        g_error_free(gerror);
        if (unlink(temp_file.c_str()) < 0)
            perror("unlink");
        return false;
    }
    g_strfreev(argv);

    temp_file_ = temp_file;
    child_pid_ = pid;
    return true;
}

void EmacsInstance::childExited(pid_t pid, int status) {
    if (child_pid_ != pid) {
        fprintf(stderr, "WARNING: Unexpected child exit (pid %d)\n", pid);
        return;
    }
    if (!callback_.get()) {
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
    GError *gerror;
    if (g_file_get_contents(temp_file_.c_str(),
                            &contents, &length,
                            &gerror)) {
        // Copy to NPN_MemAlloc-compatible buffer.
        NPUTF8 *np_contents = static_cast<NPUTF8*>(NPN_MemAlloc(length));
        memcpy(np_contents, contents, length);
        g_free(contents);
        STRINGN_TO_NPVARIANT(np_contents, length, args[0]);
    } else {
        fprintf(stderr, "Error: Failed read file: %s\n", gerror->message);
        g_error_free(gerror);
        NULL_TO_NPVARIANT(args[0]);
    }

    NPVariant result;
    NPN_InvokeDefault(parent_->npp(), callback_.get(), args, 2, &result);
    NPN_ReleaseVariantValue(&args[0]);
    NPN_ReleaseVariantValue(&args[1]);
    NPN_ReleaseVariantValue(&result);
}
