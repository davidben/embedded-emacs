#ifndef INCLUDED_TASK_H_
#define INCLUDED_TASK_H_

class Task {
  public:
    // Don't inline this to avoid emitting everywhere.
    virtual ~Task();
    virtual void run() = 0;
};

#endif  // INCLUDED_TASK_H_
