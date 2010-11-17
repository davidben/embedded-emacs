// Copyright (c) 2010 David Benjamin. All rights reserved.
// Use of this source code is governed by an MIT-style license that can be
// found in the LICENSE file.
#ifndef INCLUDED_PROCESS_WATCHER_H_
#define INCLUDED_PROCESS_WATCHER_H_

#include <sys/types.h>

class EmacsInstance;

namespace process_watcher {

void watchProcess(pid_t pid, EmacsInstance* emacs);
void killAndJoinThread();

}

#endif  // INCLUDED_PROCESS_WATCHER_H_
