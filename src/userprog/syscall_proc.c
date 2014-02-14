#include "userprog/syscall.h"
#include "userprog/syscall_handlers.h"
#include "userprog/process.h"
#include "user/syscall.h"
#include "userprog/pid.h"

static pid_t register_pid(tid_t tid) {
    return PID_ERROR;
}

void exec_handler(struct intr_frame * f) {
    char * _file_name;
    extract_params(f, &_file_name);

    tid_t _tid;
    _tid = process_execute(_file_name);

    if (_tid == TID_ERROR)
        SYSRETURN(-1);

    pid_t _pid;
    _pid = register_pid(_tid);
    SYSRETURN(_pid);
}

void wait_handler(struct intr_frame * f) {
    pid_t _pid;
    extract_params(f, &_pid);

    tid_t _tid;
    _tid = tid_of_pid(_pid);
    if (_tid == PID_ERROR)
        SYSRETURN(-1);

    SYSRETURN(process_wait(_tid));
}
