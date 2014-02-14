#include "threads/init.h"
#include "userprog/syscall.h"
#include "userprog/syscall_handlers.h"
#include "userprog/process.h"
#include "user/syscall.h"
#include "userprog/pid.h"

void exec_handler(struct intr_frame * f) {
    char * _file_name;
    extract_params(f, "s", &_file_name);

    tid_t _tid;
    _tid = process_execute(_file_name);

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
  (void)_code;

  thread_exit();
}
