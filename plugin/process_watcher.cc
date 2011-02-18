// Copyright (c) 2011 David Benjamin. All rights reserved.
// Use of this source code is governed by an MIT-style license that can be
// found in the LICENSE file.
#include "process_watcher.h"

#include <assert.h>
#include <glib.h>

#include <tr1/unordered_map>

#include "emacs_manager.h"
#include "npapi-cxx/message_proxy.h"
#include "npapi-cxx/task.h"
#include "util.h"

namespace {

class ChildExitNotifyTask : public npapi::Task {
  public:
    ChildExitNotifyTask(npapi::MessageProxy* proxy,
                        int job_id, pid_t pid, int status)
            : proxy_(proxy->ref()),
              job_id_(job_id),
              pid_(pid),
              status_(status) {
    }

    ~ChildExitNotifyTask() {
        proxy_->unref();
        proxy_ = NULL;
    }

    virtual void run(npapi::PluginInstance* instance) {
        // TODO: Get rid of this cast?
        static_cast<EmacsManager*>(instance)->childExited(job_id_, pid_, status_);
    }

  private:
    npapi::MessageProxy* proxy_;
    int job_id_;
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
        typedef std::tr1::unordered_map<int, npapi::MessageProxy*>::iterator
                iter_t;
        for (iter_t iter = job_to_npp_.begin();
             iter != job_to_npp_.end();
             ++iter) {
            iter->second->unref();
            iter->second = NULL;
        }
    }

    void watchProcess(int job_id, pid_t pid, EmacsManager* emacs) {
        assert(g_thread_self() != thread_);
        job_to_npp_[job_id] = emacs->getMessageProxy()->ref();
        GSource* source = g_child_watch_source_new(pid);
        g_source_set_callback(
            source,
            reinterpret_cast<GSourceFunc>(ProcessWatcher::childExitFunc),
            GINT_TO_POINTER(job_id), NULL);
        g_source_attach(source, context_);
        g_source_unref(source);
    }

  private:
    static void childExitFunc(GPid pid, int status, gpointer data);
 
    static gpointer threadFunction(gpointer data) {
        ProcessWatcher* p = static_cast<ProcessWatcher*>(data);
        g_main_loop_run(p->loop_);
        return NULL;
    }

    GMainContext* context_;
    GMainLoop* loop_;
    GThread* thread_;

    std::tr1::unordered_map<int, npapi::MessageProxy*> job_to_npp_;
    DISALLOW_COPY_AND_ASSIGN(ProcessWatcher);
};

ProcessWatcher* g_process_watcher = NULL;

// TODO: Be less lame and don't require a reference to g_process_watcher.
void ProcessWatcher::childExitFunc(GPid pid, int status, gpointer data) {
    int job_id = GPOINTER_TO_INT(data);
    // BAH.
    ProcessWatcher* p = g_process_watcher;
    assert(g_thread_self() == p->thread_);
    // TODO: This map is really stupid now because there's only
    // one NPP in the entire plugin anyway. Just make ProcessWatch
    // PluginInstance-specific. Write the nicer generic one later.
    if (p->job_to_npp_.find(job_id) != p->job_to_npp_.end()) {
        p->job_to_npp_[job_id]->postTask(new ChildExitNotifyTask(
            p->job_to_npp_[job_id], job_id, pid, status));
        p->job_to_npp_[job_id]->unref();
        p->job_to_npp_.erase(job_id);
    } else {
        fprintf(stderr, "WARNING: Unexpected status %d from pid %d\n",
                status, pid);
    }
}

void startWatcherThread() {
    g_process_watcher = new ProcessWatcher();
}

}  // namespace

namespace process_watcher {

void watchProcess(int job_id, pid_t pid, EmacsManager* emacs) {
    if (!g_process_watcher)
        startWatcherThread();
    return g_process_watcher->watchProcess(job_id, pid, emacs);
}

void killAndJoinThread() {
    if (!g_process_watcher)
        return;
    delete g_process_watcher;
    g_process_watcher = NULL;
}

}
