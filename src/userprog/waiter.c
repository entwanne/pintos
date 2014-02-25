#include "userprog/waiter.h"
#include "threads/malloc.h"

/* Definitions */
struct list waiters_list;
struct lock waiters_lock;
struct waiter * launching_process = NULL;
bool launching_process_loaded;

void waiters_init(void) {
    list_init(&waiters_list);
    lock_init(&waiters_lock);
}

struct waiter* find_waiter_by_child(struct thread *thr)
{
  return find_waiter_by_child_tid(thr->tid);
}

struct waiter* find_waiter_by_child_tid(tid_t tid)
{
  struct list_elem* e;
  lock_acquire(&waiters_lock);
  for (e = list_begin(&waiters_list);
       e != list_end(&waiters_list);
       e = list_next(e))
    {
      struct waiter *waiter = list_entry(e, struct waiter, elem);
      if (waiter != NULL && waiter->child_tid == tid) {
	lock_release(&waiters_lock);
	return waiter;
      }
    }
  lock_release(&waiters_lock);
  return NULL;
}

struct waiter* find_current_waiter(void)
{
  return find_waiter_by_child(thread_current());
}

void release_waiter(struct waiter* waiter, bool lock)
{
  sema_up(&waiter->ref_counter);
  if (lock)
    lock_acquire(&waiters_lock);
  if (waiter->ref_counter.value >= 2) {
    list_remove(&waiter->elem);
    free(waiter);
  }
  if (lock)
    lock_release(&waiters_lock);
}
