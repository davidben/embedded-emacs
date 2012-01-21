// Copyright (c) 2011 David Benjamin. All rights reserved.
// Use of this source code is governed by an MIT-style license that can be
// found in the LICENSE file.

#ifndef INCLUDED_EVENT_THREAD_H_
#define INCLUDED_EVENT_THREAD_H_

#include "util.h"

G_FORWARD_DECLARE(GMainContext);
G_FORWARD_DECLARE(GMainLoop);
G_FORWARD_DECLARE(GThread);

// A thread that runs a GMainLoop so we don't assume the browser has a
// glib-based event loop.
class EventThread {
 public:
  EventThread();
  ~EventThread();

  GMainContext* main_context() const { return main_context_; }

 private:
  GThread* thread_;
  GMainContext* main_context_;
  GMainLoop* main_loop_;

  static void* ThreadFunction(void* data);

  DISALLOW_COPY_AND_ASSIGN(EventThread);
};

#endif  // INCLUDED_EVENT_THREAD_H_
