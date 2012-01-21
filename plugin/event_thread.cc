// Copyright (c) 2011 David Benjamin. All rights reserved.
// Use of this source code is governed by an MIT-style license that can be
// found in the LICENSE file.

#include "event_thread.h"

#include <assert.h>
#include <glib.h>

EventThread::EventThread()
    : thread_(NULL),
      main_context_(NULL),
      main_loop_(NULL) {
  main_context_ = g_main_context_new();
  main_loop_ = g_main_loop_new(main_context_, FALSE);
  // TODO: error handling.
  thread_ = g_thread_create(EventThread::ThreadFunction, this, TRUE, NULL);
}

EventThread::~EventThread() {
  assert(g_thread_self() != thread_);
  // Stop the thread.
  g_main_loop_quit(main_loop_);
  g_thread_join(thread_);

  // Clean up.
  g_main_context_unref(main_context_);
  g_main_loop_unref(main_loop_);
}

void* EventThread::ThreadFunction(void* data) {
  EventThread* e = static_cast<EventThread*>(data);
  g_main_loop_run(e->main_loop_);
  return NULL;
}
