// Copyright (c) 2010 David Benjamin. All rights reserved.
// Use of this source code is governed by an MIT-style license that can be
// found in the LICENSE file.
#ifndef INCLUDED_TASK_H_
#define INCLUDED_TASK_H_

class Task {
  public:
    // Don't inline this to avoid emitting everywhere.
    virtual ~Task();
    virtual void run() = 0;
};

#endif  // INCLUDED_TASK_H_
