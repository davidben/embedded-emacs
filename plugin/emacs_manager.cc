// Copyright (c) 2012 David Benjamin. All rights reserved.
// Use of this source code is governed by an MIT-style license that can be
// found in the LICENSE file.

#include "emacs_manager.h"

#include <glib.h>
#include <stdio.h>

#include "emacs_instance.h"
#include "emacs_object.h"
#include "event_thread.h"
#include "npapi-cxx/browser.h"
#include "npapi-cxx/task.h"
#include "npapi-headers/npruntime.h"

namespace {

class ChildExitedTask : public npapi::Task {
 public:
  ChildExitedTask(EmacsInstance *emacs, GPid pid, int status)
      : emacs_(emacs),
        pid_(pid),
        status_(status) {
  }

  virtual void Run(npapi::PluginInstance* manager) {
    static_cast<EmacsManager*>(manager)->ChildExited(emacs_, pid_, status_);
  }

 private:
  EmacsInstance* emacs_;
  GPid pid_;
  int status_;

  DISALLOW_COPY_AND_ASSIGN(ChildExitedTask);
};

void ChildExitThunk(GPid pid, int status, gpointer data) {
  EmacsInstance* instance = static_cast<EmacsInstance*>(data);
  instance->manager()->PostTask(
      new ChildExitedTask(instance, pid, status));
}

}  // namespace

EmacsManager::EmacsManager(NPP npp)
    : npapi::PluginInstance(npp),
      next_job_id_(1),
      event_thread_(NULL) {
  // Set window-less so Chromium doesn't try to XEmbed us.
  NPN_SetValue(npp, NPPVpluginWindowBool, NULL);
}

EmacsManager::~EmacsManager() {
  // Clean up all remaining EmacsInstances
  typedef std::tr1::unordered_map<int, EmacsInstance*>::iterator iter_t;
  for (iter_t i = emacs_jobs_.begin(), end = emacs_jobs_.end();
       i != end;
       ++i) {
    delete i->second;
  }

  delete event_thread_;
}

int EmacsManager::StartEditor(long window_id,
                              EditorType editor,
			      const std::string& initial_text,
                              NPObject *callback,
                              std::string *error) {
  EmacsInstance *instance =
      new EmacsInstance(this, window_id, editor, initial_text, callback);
  if (!instance->pid()) {
    *error = instance->error();
    fprintf(stderr, "Error: %s\n", error->c_str());
    delete instance;
    return 0;
  }
  int job_id = next_job_id_++;
  instance->set_job_id(job_id);
  emacs_jobs_[job_id] = instance;

  // Watch the pid.
  if (!event_thread_)
    event_thread_ = new EventThread;
  GSource* source = g_child_watch_source_new(instance->pid());
  g_source_set_callback(
      source, reinterpret_cast<GSourceFunc>(ChildExitThunk), instance, NULL);
  g_source_attach(source, event_thread_->main_context());
  g_source_unref(source);

  return job_id;
}

void EmacsManager::ChildExited(EmacsInstance* instance, GPid pid, int status) {
  instance->ChildExited(pid, status);
  emacs_jobs_.erase(instance->job_id());
}

NPError EmacsManager::GetValue(NPPVariable variable, void* value) {
  NPError err = NPERR_NO_ERROR;
  switch (variable) {
    case NPPVpluginScriptableNPObject: {
      if (!script_object_.get()) {
        script_object_.reset(EmacsObject::Create(npp()));
      }
      *reinterpret_cast<NPObject**>(value) =
          static_cast<NPObject*>(script_object_.get());
      break;
    }
    default:
      err = PluginInstance::GetValue(variable, value);
  }
  return err;
}
