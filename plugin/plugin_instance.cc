// Copyright (c) 2011 David Benjamin. All rights reserved.
// Use of this source code is governed by an MIT-style license that can be
// found in the LICENSE file.

#include "plugin_instance.h"

#include <glib.h>

#include "message_proxy.h"
#include "task.h"

namespace {

void deleteTask(void *ptr) {
    delete static_cast<Task*>(ptr);
}

void processTasksThunk(void *ptr) {
    static_cast<PluginInstance*>(ptr)->processTasks();
}

}  // namespace

PluginInstance::PluginInstance(NPP npp)
        : npp_(npp),
          message_proxy_(NULL) {
    npp_->pdata = this;
    task_queue_ = g_async_queue_new_full(deleteTask);
}

PluginInstance::~PluginInstance() {
    if (message_proxy_) {
        message_proxy_->invalidate();
        message_proxy_->unref();
        message_proxy_ = NULL;
    }
    g_async_queue_unref(task_queue_);
    npp_->pdata = NULL;
}

NPP PluginInstance::npp() {
    return npp_;
}

MessageProxy* PluginInstance::getMessageProxy() {
    if (!message_proxy_) {
        message_proxy_ = new MessageProxy(this);
    }
    return message_proxy_;
}

void PluginInstance::postTask(Task* task) {
    if (!task) return;
    g_async_queue_push(task_queue_, task);
    // No memory is allocated, so we don't care if this runs or
    // not. It may also run more often than necessary, but that's
    // okay.
    NPN_PluginThreadAsyncCall(npp_, processTasksThunk, this);
}

void PluginInstance::processTasks() {
    void* data;
    while ((data = g_async_queue_try_pop(task_queue_))) {
        Task* task = static_cast<Task*>(data);
        task->run(this);
        delete task;
    }
}

NPError PluginInstance::setWindow(NPWindow* window) {
    return NPERR_NO_ERROR;
}

NPError PluginInstance::getValue(NPPVariable variable, void* value) {
    return NPERR_INVALID_PARAM;
}
