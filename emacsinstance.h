#ifndef INCLUDED_EMACS_INSTANCE_H_
#define INCLUDED_EMACS_INSTANCE_H_

#include "plugin.h"
#include "util.h"

class EmacsInstance {
public:
    EmacsInstance();
    ~EmacsInstance();

    NPError setWindow(NPWindow* window);
private:
    DISALLOW_COPY_AND_ASSIGN(EmacsInstance);
};

#endif  // INCLUDED_EMACS_INSTANCE_H_
