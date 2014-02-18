#include "userprog/waiter.h"
#include "threads/malloc.h"

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
      lock_acquire(&waiters_lock);
      struct waiter *waiter = list_entry(e, struct waiter, elem);
      lock_release(&waiters_lock);
      if (waiter != NULL && waiter->child == thr)
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
      lock_acquire(&waiters_lock);
      struct waiter *waiter = list_entry(e, struct waiter, elem);
      lock_release(&waiters_lock);
      if (waiter != NULL && waiter->child != NULL && waiter->child->tid == tid)
	return waiter;
    }
  return NULL;
}

struct waiter* find_current_waiter(void)
{
  return find_waiter_by_child(thread_current());
}

#include "lib/stdio.h"

void release_waiter(struct waiter* waiter)
{
  printf("===========> %d\n", waiter->ref_counter.value);
  sema_down(&waiter->ref_counter);
  printf("===========> %d\n", waiter->ref_counter.value);
  if (waiter->ref_counter.value == 0)
    {
      /* printf("111111111111\n"); */
      /* lock_acquire(&waiters_lock); */
      /* list_remove(&waiter->elem); */
      /* /\* free(waiter); *\/ */
      /* lock_release(&waiters_lock); */
      /* printf("22222222222\n"); */
    }
}
