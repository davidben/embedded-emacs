// Copyright (c) 2011 David Benjamin. All rights reserved.
// Use of this source code is governed by an MIT-style license that can be
// found in the LICENSE file.

#ifndef INCLUDED_PLUGIN_INSTANCE_H_
#define INCLUDED_PLUGIN_INSTANCE_H_

#include <string>

#include <sys/types.h>

#include "npapi-cxx/browser.h"
#include "util.h"

G_FORWARD_DECLARE(GAsyncQueue);

namespace npapi {

class Task;

class PluginInstance {
 public:
  PluginInstance(NPP npp);
  virtual ~PluginInstance();

  static PluginInstance* from_npp(NPP npp) {
    if (!npp) return NULL;
    return static_cast<PluginInstance*>(npp->pdata);
  }

  NPP npp() { return npp_; }

  // Called on any thread. Safe to call until around the end of the
  // destructor. Caller is responsible for ensuring thread lifetime
  // is shorter than PluginInstance.
  void PostTask(Task* task);
  // Called on the main plugin thread.
  void ProcessTasks();

  virtual NPError SetWindow(NPWindow* window);
  virtual NPError GetValue(NPPVariable variable, void* value);
 private:
  NPP npp_;

  GAsyncQueue* task_queue_;

  DISALLOW_COPY_AND_ASSIGN(PluginInstance);
};

}  // namespace npapi

#endif  // INCLUDED_PLUGIN_INSTANCE_H_
