// Copyright (c) 2011 David Benjamin. All rights reserved.
// Use of this source code is governed by an MIT-style license that can be
// found in the LICENSE file.
#ifndef INCLUDED_EMACS_INSTANCE_H_
#define INCLUDED_EMACS_INSTANCE_H_

#include <string>

#include <sys/types.h>

#include "npapi-cxx/browser.h"
#include "npapi-cxx/scoped_npobject.h"
#include "util.h"

class EmacsManager;
typedef struct NPObject NPObject;

class EmacsInstance {
  public:
    EmacsInstance(EmacsManager* parent,
                  long window_id,
                  const std::string& editor_command,
                  const char *initial_text, uint32_t text_len,
                  NPObject *callback);
    ~EmacsInstance();

    // Returns 0 if the launch failed.
    pid_t pid() const { return child_pid_; }
    const std::string& error() const { return error_; }

    int job_id() const { return job_id_; }
    void set_job_id(int job_id) { job_id_ = job_id; }

    EmacsManager* manager() const { return parent_; }

    void childExited(pid_t pid, int status);
private:
    bool startEditor(long window_id,
                     const std::string& editor_command,
                     const char *initial_text, uint32_t text_len);

    EmacsManager *parent_;
    std::string error_;
    pid_t child_pid_;
    npapi::scoped_npobject<NPObject> callback_;
    int job_id_;

    std::string temp_file_;

    DISALLOW_COPY_AND_ASSIGN(EmacsInstance);
};

#endif  // INCLUDED_EMACS_INSTANCE_H_
