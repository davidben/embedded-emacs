// Copyright (c) 2011 David Benjamin. All rights reserved.
// Use of this source code is governed by an MIT-style license that can be
// found in the LICENSE file.
#include "message_proxy.h"

#include <glib.h>

#include "plugin_instance.h"
#include "task.h"

namespace npapi {

MessageProxy::MessageProxy(PluginInstance* instance)
        : instance_(instance),
          ref_count_(1),
          lock_(g_mutex_new())
{
}

MessageProxy::~MessageProxy()
{
}

MessageProxy* MessageProxy::ref()
{
    g_atomic_int_inc(&ref_count_);
    return this;
}

void MessageProxy::unref()
{
    if (g_atomic_int_dec_and_test(&ref_count_))
        delete this;
}

void MessageProxy::postTask(Task* task)
{
    bool posted = false;
    g_mutex_lock(lock_);
    // TODO: If we move the message queue into the proxy, the critical
    // section would shrink; it's only really needed for the NPN call.
    if (instance_) {
        instance_->postTask(task);
        posted = true;
    }
    g_mutex_unlock(lock_);
    if (!posted)
        delete task;
}

void MessageProxy::invalidate()
{
    g_mutex_lock(lock_);
    instance_ = NULL;
    g_mutex_unlock(lock_);
}

}  // namespace npapi
