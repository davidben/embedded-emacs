// Copyright (c) 2011 David Benjamin. All rights reserved.
// Use of this source code is governed by an MIT-style license that can be
// found in the LICENSE file.
#include "process_watcher.h"

#include <assert.h>
#include <glib.h>

#include <tr1/unordered_map>

#include "emacs_instance.h"
#include "npapi-cxx/message_proxy.h"
#include "npapi-cxx/task.h"
#include "util.h"

namespace {

class ChildExitNotifyTask : public npapi::Task {
  public:
    ChildExitNotifyTask(npapi::MessageProxy* proxy, pid_t pid, int status)
            : proxy_(proxy->ref()),
              pid_(pid),
              status_(status) {
    }

    ~ChildExitNotifyTask() {
        proxy_->unref();
        proxy_ = NULL;
    }

    virtual void run(npapi::PluginInstance* instance) {
        // TODO: Get rid of this ridiculous cast.
        static_cast<EmacsInstance*>(instance)->childExited(pid_, status_);
    }

  private:
    npapi::MessageProxy* proxy_;
    pid_t pid_;
    int status_;
};

class ProcessWatcher {
  public:
    ProcessWatcher() {
        context_ = g_main_context_new();
        loop_ = g_main_loop_new(context_, FALSE);
        thread_ = g_thread_create(ProcessWatcher::threadFunction,
                                  this, TRUE, NULL);
    }

    ~ProcessWatcher() {
        assert(g_thread_self() != thread_);
        // Stop the thread.
        g_main_loop_quit(loop_);
        g_thread_join(thread_);

        // Clean up.
        g_main_context_unref(context_);
        g_main_loop_unref(loop_);

        // Unref any message proxies still alive.
        typedef std::tr1::unordered_map<pid_t, npapi::MessageProxy*>::iterator
                iter_t;
        for (iter_t iter = pid_to_npp_.begin();
             iter != pid_to_npp_.end();
             ++iter) {
            iter->second->unref();
            iter->second = NULL;
        }
    }

    void watchProcess(pid_t pid, EmacsInstance* emacs) {
        assert(g_thread_self() != thread_);
        if (pid_to_npp_.find(pid) != pid_to_npp_.end()) {
            fprintf(stderr, "Error: %ld is already watched.\n", (long)pid);
            return;
        }
        pid_to_npp_[pid] = emacs->getMessageProxy()->ref();
        GSource* source = g_child_watch_source_new(pid);
        g_source_set_callback(
            source,
            reinterpret_cast<GSourceFunc>(ProcessWatcher::childExitFunc),
            this, NULL);
        g_source_attach(source, context_);
        g_source_unref(source);
    }

  private:
    static void childExitFunc(GPid pid, int status, gpointer data) {
        ProcessWatcher* p = static_cast<ProcessWatcher*>(data);
        assert(g_thread_self() == p->thread_);
        if (p->pid_to_npp_.find(pid) != p->pid_to_npp_.end()) {
            p->pid_to_npp_[pid]->postTask(new ChildExitNotifyTask(
                p->pid_to_npp_[pid], pid, status));
            p->pid_to_npp_[pid]->unref();
            p->pid_to_npp_.erase(pid);
	} else {
            fprintf(stderr, "WARNING: Unexpected status %d from pid %d\n",
                    status, pid);
        }
    }

    static gpointer threadFunction(gpointer data) {
        ProcessWatcher* p = static_cast<ProcessWatcher*>(data);
        g_main_loop_run(p->loop_);
        return NULL;
    }

    GMainContext* context_;
    GMainLoop* loop_;
    GThread* thread_;

    std::tr1::unordered_map<pid_t, npapi::MessageProxy*> pid_to_npp_;
    DISALLOW_COPY_AND_ASSIGN(ProcessWatcher);
};

ProcessWatcher* g_process_watcher = NULL;

void startWatcherThread() {
    g_process_watcher = new ProcessWatcher();
}

}  // namespace

namespace process_watcher {

void watchProcess(pid_t pid, EmacsInstance* emacs)
{
    if (!g_process_watcher)
        startWatcherThread();
    return g_process_watcher->watchProcess(pid, emacs);
}

void killAndJoinThread()
{
    if (!g_process_watcher)
        return;
    delete g_process_watcher;
    g_process_watcher = NULL;
}

}
