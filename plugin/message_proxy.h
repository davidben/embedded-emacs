// Copyright (c) 2010 David Benjamin. All rights reserved.
// Use of this source code is governed by an MIT-style license that can be
// found in the LICENSE file.
#ifndef INCLUDED_MESSAGE_PROXY_H_
#define INCLUDED_MESSAGE_PROXY_H_

#include "util.h"

class EmacsInstance;
G_FORWARD_DECLARE(GMutex);
class Task;

// FIXME: This needs some serious refactoring, but it should /work/.
class MessageProxy {
  public:
    // NOTE: THIS SHOULD ONLY BE CALLED BY EmacsInstance.
    MessageProxy(EmacsInstance* instance);

    MessageProxy* ref();
    void unref();

    void postTask(Task* task);
    // Only called on the plugin thread.
    void invalidate();

  private:
    ~MessageProxy();

    EmacsInstance* instance_;
    volatile int ref_count_;

    GMutex* lock_;

    DISALLOW_COPY_AND_ASSIGN(MessageProxy);
};

#endif  // INCLUDED_MESSAGE_PROXY_H_
