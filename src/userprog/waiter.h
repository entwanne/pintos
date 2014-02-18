#ifndef __WAITER_H__
#define __WAITER_H__

#include "threads/thread.h"

struct waiter {
  struct thread* parent;
  struct thread* child;
  // + ref_counter, status_code
};

extern struct waiter* launching_process;

#endif
