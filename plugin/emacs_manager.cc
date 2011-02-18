// Copyright (c) 2011 David Benjamin. All rights reserved.
// Use of this source code is governed by an MIT-style license that can be
// found in the LICENSE file.
#include "emacs_manager.h"

#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <string>
#include <sstream>

#include "emacs_instance.h"
#include "emacs_object.h"
#include "npapi-cxx/browser.h"
#include "npapi-headers/npruntime.h"
#include "process_watcher.h"

EmacsManager::EmacsManager(NPP npp)
        : npapi::PluginInstance(npp),
          next_job_id_(1) {
    // Set window-less so Chromium doesn't try to XEmbed us.
    // NOTE: Chromium source says Mozilla documentation is lying; NULL
    // is true for both Mozilla and Chromium.
    NPN_SetValue(npp, NPPVpluginWindowBool, NULL);
}

EmacsManager::~EmacsManager() {
    // Clean up all remaining EmacsInstances
    typedef std::tr1::unordered_map<int, EmacsInstance*>::iterator iter_t;
    for (iter_t i = emacs_jobs_.begin(), end = emacs_jobs_.end();
	 i != end;
	 ++i) {
        delete i->second;
    }
}

int EmacsManager::startEditor(long windowId,
                              const char *initialText, uint32_t textLen,
                              NPObject *callback,
                              std::string *error) {
    EmacsInstance *instance =
            new EmacsInstance(this, windowId, editor_command_,
                              initialText, textLen, callback);
    if (!instance->pid()) {
        *error = instance->error();
        delete instance;
        return 0;
    }
    int job_id = next_job_id_++;
    emacs_jobs_[job_id] = instance;
    process_watcher::watchProcess(job_id, instance->pid(), this);
    return job_id;
}

void EmacsManager::setEditorCommand(const char *utf8Chars, uint32_t len) {
    editor_command_.assign(utf8Chars, len);
}

void EmacsManager::childExited(int job_id, pid_t pid, int status) {
    typedef std::tr1::unordered_map<int, EmacsInstance*>::iterator iter_t;
    iter_t iter = emacs_jobs_.find(job_id);
    if (iter == emacs_jobs_.end()) {
        fprintf(stderr, "WARNING: Unexpected child exit (pid %d, job_id %d)\n",
                pid, job_id);
        return;
    }
    iter->second->childExited(pid, status);
    delete iter->second;
    emacs_jobs_.erase(iter);
}

NPError EmacsManager::getValue(NPPVariable variable, void* value) {
    NPError err = NPERR_NO_ERROR;
    switch (variable) {
        case NPPVpluginScriptableNPObject: {
            if (!script_object_.get()) {
                script_object_.reset(EmacsObject::create(npp()));
            }
            *reinterpret_cast<NPObject**>(value) =
                    static_cast<NPObject*>(script_object_.get());
            break;
        }
        default:
            err = PluginInstance::getValue(variable, value);
    }
    return err;
}
