// Copyright (c) 2012 David Benjamin. All rights reserved.
// Use of this source code is governed by an MIT-style license that can be
// found in the LICENSE file.

#ifndef INCLUDED_EMACS_MANAGER_H_
#define INCLUDED_EMACS_MANAGER_H_

#include <tr1/unordered_map>
#include <string>

#include <sys/types.h>

#include "editor_type.h"
#include "npapi-cxx/plugin_instance.h"
#include "npapi-cxx/scoped_npobject.h"
#include "util.h"

class EmacsInstance;
class EmacsObject;
class EventThread;
typedef struct NPObject NPObject;

// There really should only ever be one instance of this class.
class EmacsManager : public npapi::PluginInstance {
 public:
  EmacsManager(NPP npp);
  ~EmacsManager();

  int StartEditor(long window_id,
                  EditorType editor,
                  const std::string& initial_text,
                  NPObject *callback,
                  std::string *error);
  // TODO: Implement some way to cancel a job?

  void ChildExited(EmacsInstance* instance, pid_t pid, int status);

  NPError GetValue(NPPVariable variable, void* value);
 private:
  int next_job_id_;
  npapi::scoped_npobject<EmacsObject> script_object_;
  std::tr1::unordered_map<int, EmacsInstance*> emacs_jobs_;

  EventThread *event_thread_;

  DISALLOW_COPY_AND_ASSIGN(EmacsManager);
};

#endif  // INCLUDED_EMACS_MANAGER_H_
