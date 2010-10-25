#ifndef INCLUDED_MESSAGE_PROXY_H_
#define INCLUDED_MESSAGE_PROXY_H_

#include "util.h"

class EmacsInstance;
G_FORWARD_DECLARE(GMutex);
class Task;

class MessageProxy {
  public:
    // NOTE: THIS SHOULD ONLY BE CALLED BY EmacsInstance.
    MessageProxy(EmacsInstance* instance);

    MessageProxy* ref();
    void unref();

    void postTask(Task* task);
    void invalidate();

  private:
    ~MessageProxy();

    EmacsInstance* instance_;
    volatile int ref_count_;

    GMutex* lock_;

    DISALLOW_COPY_AND_ASSIGN(MessageProxy);
};

#endif  // INCLUDED_MESSAGE_PROXY_H_
