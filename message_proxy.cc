#include "message_proxy.h"

#include <glib.h>

#include "emacsinstance.h"

MessageProxy::MessageProxy(EmacsInstance* instance)
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
    g_mutex_lock(lock_);
    // TODO: If we move the message queue into the proxy, the critical
    // section would shrink; it's only really needed for the NPN call.
    if (instance_) {
        instance_->postTask(task);
    }
    g_mutex_unlock(lock_);
}

void MessageProxy::invalidate()
{
    g_mutex_lock(lock_);
    instance_ = NULL;
    g_mutex_unlock(lock_);
}
