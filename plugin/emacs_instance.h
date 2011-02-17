// Copyright (c) 2011 David Benjamin. All rights reserved.
// Use of this source code is governed by an MIT-style license that can be
// found in the LICENSE file.
#ifndef INCLUDED_EMACS_INSTANCE_H_
#define INCLUDED_EMACS_INSTANCE_H_

#include <string>

#include <sys/types.h>

#include "browser.h"
#include "plugin_instance.h"
#include "util.h"

class EmacsObject;

class EmacsInstance : public PluginInstance {
public:
    EmacsInstance(NPP npp);
    ~EmacsInstance();

    // FIXME: to avoid possible race conditions (can the script object
    // get created before the window?), always succeed here and just
    // launch this emacs object when we get the window.
    bool startEditor(std::string *error);
    void setCallback(NPObject* callback);
    void setInitialText(const char *utf8Chars, uint32_t len);
    void setEditorCommand(const char *utf8Chars, uint32_t len);

    // process_watcher calls this function when a child exitted.
    void childExited(pid_t pid, int status);

    NPError setWindow(NPWindow* window);
    NPError getValue(NPPVariable variable, void* value);
private:
    long window_id_;  // Should I include X11 header files and use
                      // Window?
    pid_t child_pid_;
    EmacsObject* script_object_;
    NPObject* callback_;
    std::string initial_text_;  // UTF-8 encoded
    std::string editor_command_;  // UTF-8 encoded
    std::string temp_file_;

    DISALLOW_COPY_AND_ASSIGN(EmacsInstance);
};

#endif  // INCLUDED_EMACS_INSTANCE_H_
