#ifndef __WAITER_H__
#define __WAITER_H__

#include "stdbool.h"
#include "threads/thread.h"
#include "kernel/list.h"
#include "threads/synch.h"

struct waiter {
  struct thread* parent;
  struct thread* child;
  // + ref_counter, status_code
  bool is_waiting;
  struct list_elem elem;
};

extern struct waiter* launching_process;

extern struct list waiters_list;
extern struct lock waiters_lock;

void waiters_init(void);

extern struct waiter* launching_process;

#endif
