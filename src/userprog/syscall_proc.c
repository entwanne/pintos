#include "threads/init.h"
#include "userprog/syscall.h"
#include "userprog/syscall_handlers.h"
#include "userprog/process.h"
#include "user/syscall.h"
#include "userprog/pid.h"
#include "threads/synch.h"
#include "userprog/waiter.h"
#include "threads/malloc.h"

void exec_handler(struct intr_frame * f) {
  char * _command;
  extract_params(f, "s", &_command);

  tid_t _tid = process_execute(_command);

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
