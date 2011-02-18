// Copyright (c) 2011 David Benjamin. All rights reserved.
// Use of this source code is governed by an MIT-style license that can be
// found in the LICENSE file.
#ifndef INCLUDED_EMACS_MANAGER_H_
#define INCLUDED_EMACS_MANAGER_H_

#include <tr1/unordered_map>
#include <string>

#include <sys/types.h>

#include "npapi-cxx/plugin_instance.h"
#include "npapi-cxx/scoped_npobject.h"
#include "util.h"

class EmacsInstance;
class EmacsObject;
typedef struct NPObject NPObject;

// There really should only ever be one instance of this class.
class EmacsManager : public npapi::PluginInstance {
public:
    EmacsManager(NPP npp);
    ~EmacsManager();

    int startEditor(long windowId,
		    const char *initialText, uint32_t textLen,
		    NPObject *callback,
		    std::string *error);
    // TODO: Implement some way to cancel a job?

    void setEditorCommand(const char *utf8Chars, uint32_t len);

    // process_watcher calls this function when a child exitted.
    void childExited(int job_id, pid_t pid, int status);

    NPError getValue(NPPVariable variable, void* value);
private:
    int next_job_id_;
    npapi::scoped_npobject<EmacsObject> script_object_;
    std::string editor_command_;  // UTF-8 encoded
    std::tr1::unordered_map<int, EmacsInstance*> emacs_jobs_;

    DISALLOW_COPY_AND_ASSIGN(EmacsManager);
};

#endif  // INCLUDED_EMACS_MANAGER_H_
