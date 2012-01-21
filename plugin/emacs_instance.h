// Copyright (c) 2011 David Benjamin. All rights reserved.
// Use of this source code is governed by an MIT-style license that can be
// found in the LICENSE file.

#ifndef INCLUDED_EMACS_INSTANCE_H_
#define INCLUDED_EMACS_INSTANCE_H_

#include <string>

#include <glib.h>

#include "editor_type.h"
#include "npapi-cxx/browser.h"
#include "npapi-cxx/scoped_npobject.h"
#include "util.h"

class EmacsManager;
G_FORWARD_DECLARE(GtkWidget);
typedef struct NPObject NPObject;

class EmacsInstance {
 public:
  EmacsInstance(EmacsManager* parent,
                long window_id,
                EditorType editor,
                const std::string& initial_text,
                NPObject *callback);
  ~EmacsInstance();

  // Returns 0 if the launch failed.
  GPid pid() const { return child_pid_; }
  const std::string& error() const { return error_; }

  int job_id() const { return job_id_; }
  void set_job_id(int job_id) { job_id_ = job_id; }

  EmacsManager* manager() const { return parent_; }

  void ChildExited(GPid pid, int status);
 private:
  bool StartEditor(long window_id,
                   EditorType editor,
                   const std::string& initial_text);

  EmacsManager *parent_;
  std::string error_;
  GPid child_pid_;
  npapi::scoped_npobject<NPObject> callback_;
  int job_id_;
  GtkWidget* plug_;

  std::string temp_file_;

  DISALLOW_COPY_AND_ASSIGN(EmacsInstance);
};

#endif  // INCLUDED_EMACS_INSTANCE_H_
