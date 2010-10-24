#include "emacsinstance.h"

#include <unistd.h>

#include <string>
#include <sstream>

EmacsInstance::EmacsInstance(NPP npp)
        : npp_(npp),
          window_id_(0),
          child_pid_(0),
          script_object_(NULL)
{
}

EmacsInstance::~EmacsInstance()
{
}

NPError EmacsInstance::setWindow(NPWindow* window)
{
    long window_id = reinterpret_cast<long>(window->window);
    if (!window_id_) {
        std::stringstream ss;
        ss << window_id;
        std::string window_str = ss.str();

        pid_t pid = fork();
        if (pid < 0) {
            return NPERR_GENERIC_ERROR;
        } else if (pid == 0) {
            // In the child. NO MALLOC AFTER THIS POINT.
            execlp("emacs", "emacs", "--parent-id", window_str.c_str(), NULL);
            _exit(1);
        }

        child_pid_ = pid;
        window_id_ = window_id;
    } else {
        // This really shouldn't happen.
        // TODO: Notify javascript about it.
        if (window_id != window_id_)
            return NPERR_GENERIC_ERROR;
    }
    return NPERR_NO_ERROR;
}

NPObject* EmacsInstance::getScriptObject()
{
    // TODO
    return NULL;
}
