// Copyright (c) 2011 David Benjamin. All rights reserved.
// Use of this source code is governed by an MIT-style license that can be
// found in the LICENSE file.
#ifndef INCLUDED_MESSAGE_PROXY_H_
#define INCLUDED_MESSAGE_PROXY_H_

#include "util.h"

G_FORWARD_DECLARE(GMutex);

namespace npapi {

class PluginInstance;
class Task;

// FIXME: This needs some serious refactoring, but it should /work/.
class MessageProxy {
  public:
    // NOTE: THIS SHOULD ONLY BE CALLED BY PluginInstance.
    MessageProxy(PluginInstance* instance);

    MessageProxy* ref();
    void unref();

    void postTask(Task* task);
    // Only called on the plugin thread.
    void invalidate();

  private:
    ~MessageProxy();

    PluginInstance* instance_;
    volatile int ref_count_;

    GMutex* lock_;

    DISALLOW_COPY_AND_ASSIGN(MessageProxy);
};

}  // namespace npapi

#endif  // INCLUDED_MESSAGE_PROXY_H_
