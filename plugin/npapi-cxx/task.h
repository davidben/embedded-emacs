// Copyright (c) 2011 David Benjamin. All rights reserved.
// Use of this source code is governed by an MIT-style license that can be
// found in the LICENSE file.
#ifndef INCLUDED_TASK_H_
#define INCLUDED_TASK_H_

namespace npapi {

class PluginInstance;

class Task {
  public:
    // Don't inline this to avoid emitting everywhere.
    virtual ~Task();
    virtual void run(PluginInstance* instance) = 0;
};

}  // namespace npapi

#endif  // INCLUDED_TASK_H_
