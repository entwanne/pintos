#include "userprog/waiter.h"

/* Definitions */
struct list waiters_list;
struct lock waiters_lock;
struct waiter * launching_process = NULL;

void waiters_init(void) {
    list_init(&waiters_list);
    lock_init(&waiters_lock);
}

struct waiter* find_waiter_by_child(struct thread *thr)
{
  struct list_elem* e;
  for (e = list_begin(&waiters_list);
       e != list_end(&waiters_list);
       e = list_next(e))
    {
      struct waiter *waiter = list_entry(e, struct waiter, elem);
      if (waiter->child == thr)
	return waiter;
    }
  return NULL;
}

struct waiter* find_waiter_by_child_tid(tid_t tid)
{
  struct list_elem* e;
  for (e = list_begin(&waiters_list);
       e != list_end(&waiters_list);
       e = list_next(e))
    {
      struct waiter *waiter = list_entry(e, struct waiter, elem);
      if (waiter->child != NULL && waiter->child->tid == tid)
	return waiter;
    }
  return NULL;
}

struct waiter* find_current_waiter(void)
{
  return find_waiter_by_child(thread_current());
}
