#include "threads/init.h"
#include "userprog/syscall.h"
#include "userprog/syscall_handlers.h"
#include "userprog/process.h"
#include "user/syscall.h"
#include "userprog/pid.h"
#include "threads/synch.h"
#include "userprog/waiter.h"
#include "threads/malloc.h"

static void init_exec_lock(struct lock* lock)
{
  static int done;
  if (!done)
    lock_init(lock);
  done = 1;
}

void exec_handler(struct intr_frame * f) {
  static struct lock lock;
  char * _file_name;
  init_exec_lock(&lock);
  extract_params(f, "s", &_file_name);

  lock_acquire(&lock);
  launching_process = malloc(sizeof(*launching_process));
  launching_process->parent = thread_current();
  launching_process->child = NULL;
  launching_process->is_waiting = false;
  enum intr_level old_level;
  tid_t _tid;
  _tid = process_execute(_file_name);
  old_level = intr_disable();
  thread_block();
  intr_set_level(old_level);
  lock_release(&lock);

  if (_tid == TID_ERROR)
    SYSRETURN(-1);

  SYSRETURN(pid_of_tid(_tid));
}

void wait_handler(struct intr_frame * f) {
  pid_t _pid;
  extract_params(f, "i", &_pid);

  tid_t _tid;
  _tid = tid_of_pid(_pid);
  if (_tid == TID_ERROR)
    SYSRETURN(-1);

  SYSRETURN(process_wait(_tid));
}

void exit_handler(struct intr_frame *f) {
  int _code;
  extract_params(f, "i", &_code);
  struct thread * thr = thread_current();
  thr->exit_status = _code;

  thread_exit();
}
