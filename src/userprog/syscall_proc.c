#include "userprog/syscall.h"
#include "userprog/syscall_handlers.h"

void exec_handler(struct intr_frame * f) {
    SYSRETURN(-1);
}

void wait_handler(struct intr_frame * f) {
    SYSRETURN(-1);
}
