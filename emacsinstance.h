#ifndef INCLUDED_EMACS_INSTANCE_H_
#define INCLUDED_EMACS_INSTANCE_H_

#include <sys/types.h>

#include "browser.h"
#include "util.h"

typedef struct NPObject NPObject;
class ScriptObject;

class EmacsInstance {
public:
    EmacsInstance(NPP npp);
    ~EmacsInstance();

    // FIXME: to avoid possible race conditions (can the script object
    // get created before the window?), always succeed here and just
    // launch this emacs object when we get the window.
    bool startEditor();
    void setCallback(NPObject* callback);


    NPError setWindow(NPWindow* window);
    NPObject* getScriptObject();
private:
    NPP npp_;
    long window_id_;  // Should I include X11 header files and use
                      // Window?
    pid_t child_pid_;
    ScriptObject* script_object_;
    NPObject* callback_;
    DISALLOW_COPY_AND_ASSIGN(EmacsInstance);
};

#endif  // INCLUDED_EMACS_INSTANCE_H_
