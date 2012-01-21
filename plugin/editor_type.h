// Copyright (c) 2012 David Benjamin. All rights reserved.
// Use of this source code is governed by an MIT-style license that can be
// found in the LICENSE file.

#ifndef INCLUDED_EDITOR_TYPE_H_
#define INCLUDED_EDITOR_TYPE_H_

#include <string>

enum EditorType {
  EDITOR_TYPE_INVALID,
  EDITOR_TYPE_EMACS,
  EDITOR_TYPE_GVIM,
  EDITOR_TYPE_CUSTOM,
};

EditorType StringToEditorType(const std::string& editor);

char** GetEditorCommandArgv(EditorType editor,
                            long window_id,
                            const std::string& tmpfile_path);

#endif  // INCLUDED_EDITOR_TYPE_H_
