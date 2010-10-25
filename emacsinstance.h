#ifndef INCLUDED_EMACS_INSTANCE_H_
#define INCLUDED_EMACS_INSTANCE_H_

#include <string>

#include <sys/types.h>

#include "browser.h"
#include "util.h"

typdef struct _GAsyncQueue GAsyncQueue;
typedef struct NPObject NPObject;
class ScriptObject;
class Task;

class EmacsInstance {
public:
    EmacsInstance(NPP npp);
    ~EmacsInstance();

    // FIXME: to avoid possible race conditions (can the script object
    // get created before the window?), always succeed here and just
    // launch this emacs object when we get the window.
    bool startEditor();
    void setCallback(NPObject* callback);
    void setInitialText(const char *utf8Chars, uint32_t len);

    // Called on any thread. Safe to call until around the end of the
    // destructor. Specifically, EmacsInstance should inform everyone
    // with a reference not to post more tasks before deleting the
    // queue.
    void postTask(Task* task);
    // Called on the main plugin thread.
    void processTasks();

    NPError setWindow(NPWindow* window);
    NPObject* getScriptObject();
private:
    NPP npp_;
    long window_id_;  // Should I include X11 header files and use
                      // Window?
    pid_t child_pid_;
    ScriptObject* script_object_;
    NPObject* callback_;
    std::string initial_text_;  // UTF-8 encoded
    std::string temp_file_;

    GAsyncQueue* task_queue_;

    DISALLOW_COPY_AND_ASSIGN(EmacsInstance);
};

#endif  // INCLUDED_EMACS_INSTANCE_H_
