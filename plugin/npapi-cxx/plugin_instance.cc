// Copyright (c) 2011 David Benjamin. All rights reserved.
// Use of this source code is governed by an MIT-style license that can be
// found in the LICENSE file.

#include "plugin_instance.h"

#include <glib.h>

#include "task.h"

namespace {

void DeleteTaskThunk(void *ptr) {
  delete static_cast<npapi::Task*>(ptr);
}

void ProcessTasksThunk(void *ptr) {
  static_cast<npapi::PluginInstance*>(ptr)->ProcessTasks();
}

}  // namespace

namespace npapi {

PluginInstance::PluginInstance(NPP npp)
    : npp_(npp) {
  npp_->pdata = this;
  task_queue_ = g_async_queue_new_full(DeleteTaskThunk);
}

PluginInstance::~PluginInstance() {
  g_async_queue_unref(task_queue_);
  npp_->pdata = NULL;
}

void PluginInstance::PostTask(Task* task) {
  if (!task) return;
  g_async_queue_push(task_queue_, task);
  // No memory is allocated, so we don't care if this runs or
  // not. It may also run more often than necessary, but that's
  // okay.
  NPN_PluginThreadAsyncCall(npp_, ProcessTasksThunk, this);
}

void PluginInstance::ProcessTasks() {
  void* data;
  while ((data = g_async_queue_try_pop(task_queue_))) {
    Task* task = static_cast<Task*>(data);
    task->Run(this);
    delete task;
  }
}

NPError PluginInstance::SetWindow(NPWindow* window) {
  return NPERR_NO_ERROR;
}

NPError PluginInstance::GetValue(NPPVariable variable, void* value) {
  return NPERR_INVALID_PARAM;
}

}  // namespace npapi
