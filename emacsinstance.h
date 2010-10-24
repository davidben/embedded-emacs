#ifndef INCLUDED_EMACS_INSTANCE_H_
#define INCLUDED_EMACS_INSTANCE_H_

#include <sys/types.h>

#include "browser.h"
#include "util.h"

class EmacsInstance {
public:
    EmacsInstance();
    ~EmacsInstance();

    NPError setWindow(NPWindow* window);
private:
    long window_id_;  // Should I include X11 header files and use
                      // Window?
    pid_t child_pid_;
    DISALLOW_COPY_AND_ASSIGN(EmacsInstance);
};

#endif  // INCLUDED_EMACS_INSTANCE_H_
