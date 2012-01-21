// Copyright (c) 2012 David Benjamin. All rights reserved.
// Use of this source code is governed by an MIT-style license that can be
// found in the LICENSE file.

#include "editor_type.h"

#include <glib.h>
#include <stdio.h>

#include <sstream>

namespace {

std::string LongToString(long n) {
  std::stringstream ss;
  ss << n;
  return ss.str();
}

}  // namespace

EditorType StringToEditorType(const std::string& editor) {
  if (editor == "emacs") {
    return EDITOR_TYPE_EMACS;
  } else if (editor == "gvim") {
    return EDITOR_TYPE_GVIM;
  } else if (editor == "custom") {
    return EDITOR_TYPE_CUSTOM;
  } else {
    return EDITOR_TYPE_INVALID;
  }
}

char** GetEditorCommandArgv(EditorType editor,
                            long window_id,
                            const std::string& tmpfile_path) {
  std::string window_str = LongToString(window_id);
  switch (editor) {
    case EDITOR_TYPE_EMACS: {
      // emacs --parent-id $WINDOW -- $PATH
      char **argv = g_new(char*, 6);
      argv[0] = g_strdup("emacs");
      argv[1] = g_strdup("--parent-id");
      argv[2] = g_strdup(window_str.c_str());
      argv[3] = g_strdup("--");
      argv[4] = g_strdup(tmpfile_path.c_str());
      argv[5] = NULL;
      return argv;
    }
    case EDITOR_TYPE_GVIM: {
      // gvim --nofork --socketid $WINDOW -- $PATH
      char **argv = g_new(char*, 7);
      argv[0] = g_strdup("gvim");
      argv[1] = g_strdup("--nofork");
      argv[2] = g_strdup("--socketid");
      argv[3] = g_strdup(window_str.c_str());
      argv[4] = g_strdup("--");
      argv[5] = g_strdup(tmpfile_path.c_str());
      argv[6] = NULL;
      return argv;
    }
    case EDITOR_TYPE_CUSTOM: {
      // embedded-text-editor $WINDOW $PATH
      char **argv = g_new(char*, 4);
      argv[0] = g_strdup("embedded-text-editor");
      argv[1] = g_strdup(window_str.c_str());
      argv[2] = g_strdup(tmpfile_path.c_str());
      argv[3] = NULL;
      return argv;
    }
    default:
      fprintf(stderr, "Invalid EditorType (%d)\n", editor);
      return NULL;
  }
}
