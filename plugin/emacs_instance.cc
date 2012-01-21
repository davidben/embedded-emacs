// Copyright (c) 2011 David Benjamin. All rights reserved.
// Use of this source code is governed by an MIT-style license that can be
// found in the LICENSE file.

#include "emacs_instance.h"

#include <glib.h>
#include <gtk/gtk.h>
#include <stdio.h>
#include <string.h>

#include "emacs_manager.h"
#include "npapi-cxx/browser.h"

EmacsInstance::EmacsInstance(EmacsManager* parent,
                             long window_id,
                             EditorType editor,
                             const std::string& initial_text,
                             NPObject *callback)
    : parent_(parent),
      child_pid_(0),
      job_id_(0),
      plug_(NULL) {
  if (StartEditor(window_id, editor, initial_text)) {
    callback_.reset(callback);
  }
}

EmacsInstance::~EmacsInstance() {
  if (!temp_file_.empty()) {
    if (unlink(temp_file_.c_str()) < 0)
      perror("unlink");
  }

  if (plug_)
    gtk_widget_destroy(plug_);
}

bool EmacsInstance::StartEditor(long window_id,
                                EditorType editor,
                                const std::string& initial_text) {
  GError *gerror = NULL;

  if (!window_id) {
    fprintf(stderr, "Error: Invalid window.\n");
    error_ = "invalid window";
    return false;
  }

  // FIXME: Do file io on a dedicated thread.
  char *name = NULL;
  int fd = g_file_open_tmp(".embedded-emacs.XXXXXX", &name, &gerror);
  if (fd < 0) {
    error_ = "failed to create temporary file: ";
    error_ += gerror->message;
    return false;
  }
  temp_file_.assign(name);
  g_free(name);
  FILE* file = fdopen(fd, "w");
  if (!file) {
    close(fd);
    // FIXME: strerror_r and UNIX are a trainwreck.
    error_ = "failed to fdopen";
    return false;
  }
  fwrite(initial_text.c_str(), initial_text.size(), 1, file);
  fclose(file);

  // HACK: Wrap in socket so we can grab the focus. Remove this code
  // when http://crbug.com/27868 is fixed.
  plug_ = gtk_plug_new(window_id);
  GtkWidget* socket = gtk_socket_new();
  gtk_container_add(GTK_CONTAINER(plug_), socket);
  gtk_widget_show_all(plug_);
  gtk_widget_set_can_focus(socket, TRUE);
  gtk_widget_grab_focus(socket);
  gtk_widget_set_can_focus(socket, FALSE);

  char **argv = GetEditorCommandArgv(
      editor, gtk_socket_get_id(GTK_SOCKET(socket)), temp_file_);
  GPid pid;
  if (!g_spawn_async(NULL, argv, NULL,
                     static_cast<GSpawnFlags>(
                         G_SPAWN_DO_NOT_REAP_CHILD | G_SPAWN_SEARCH_PATH),
                     NULL, NULL, &pid, &gerror)) {
    g_strfreev(argv);
    error_ = std::string("failed to spawn child process: ") +
        gerror->message;
    g_error_free(gerror);
    return false;
  }
  g_strfreev(argv);

  child_pid_ = pid;
  return true;
}

void EmacsInstance::ChildExited(GPid pid, int status) {
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
