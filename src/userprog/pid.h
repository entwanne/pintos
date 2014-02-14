#ifndef __PID_H__
#define __PID_H__

#include "lib/user/syscall.h"
#include "threads/thread.h"

pid_t       pid_of_tid(tid_t);
tid_t       tid_of_pid(pid_t);

#endif
