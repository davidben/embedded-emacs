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

    NPError setWindow(NPWindow* window);
    NPObject* getScriptObject();
private:
    NPP npp_;
    long window_id_;  // Should I include X11 header files and use
                      // Window?
    pid_t child_pid_;
    ScriptObject* script_object_;
    DISALLOW_COPY_AND_ASSIGN(EmacsInstance);
};

#endif  // INCLUDED_EMACS_INSTANCE_H_
