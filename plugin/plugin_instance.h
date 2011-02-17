// Copyright (c) 2011 David Benjamin. All rights reserved.
// Use of this source code is governed by an MIT-style license that can be
// found in the LICENSE file.
#ifndef INCLUDED_PLUGIN_INSTANCE_H_
#define INCLUDED_PLUGIN_INSTANCE_H_

#include <string>

#include <sys/types.h>

#include "browser.h"
#include "util.h"

G_FORWARD_DECLARE(GAsyncQueue);

namespace npapi {

class MessageProxy;
class Task;

class PluginInstance {
  public:
    PluginInstance(NPP npp);
    virtual ~PluginInstance();

    static PluginInstance* fromNPP(NPP npp) {
        if (!npp) return NULL;
        return static_cast<PluginInstance*>(npp->pdata);
    }

    NPP npp();

    // Only call this on the plugin thread.
    MessageProxy* getMessageProxy();
    // Called on any thread. Safe to call until around the end of the
    // destructor. Specifically, EmacsInstance should inform everyone
    // with a reference not to post more tasks before deleting the
    // queue.
    void postTask(Task* task);
    // Called on the main plugin thread.
    void processTasks();

    virtual NPError setWindow(NPWindow* window);
    virtual NPError getValue(NPPVariable variable, void* value);
  private:
    NPP npp_;

    MessageProxy* message_proxy_;
    GAsyncQueue* task_queue_;

    DISALLOW_COPY_AND_ASSIGN(PluginInstance);
};

}  // namespace npapi

#endif  // INCLUDED_PLUGIN_INSTANCE_H_
