#ifndef __WAITER_H__
#define __WAITER_H__

#include "stdbool.h"
#include "threads/thread.h"
#include "kernel/list.h"
#include "threads/synch.h"

struct waiter {
  struct thread* parent;
  struct thread* child;
  int exit_status;
  struct semaphore ref_counter;
  bool is_waiting;
  struct list_elem elem;
};

extern struct waiter* launching_process;

extern struct list waiters_list;
extern struct lock waiters_lock;

void waiters_init(void);

extern struct waiter* launching_process;

struct waiter* find_waiter_by_child(struct thread *thr);
struct waiter* find_waiter_by_child_tid(tid_t tid);
struct waiter* find_current_waiter(void);

void release_waiter(struct waiter*);

#endif
