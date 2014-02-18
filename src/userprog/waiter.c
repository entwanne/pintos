#include "userprog/waiter.h"

/* Definitions */
struct list waiters_list;
struct lock waiters_lock;
struct waiter * launching_process = NULL;

void waiters_init(void) {
    list_init(&waiters_list);
    lock_init(&waiters_lock);
}
