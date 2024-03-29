#ifndef USERPROG_SYSCALL_HANDLERS_H
#define USERPROG_SYSCALL_HANDLERS_H

#include "threads/interrupt.h"

#define SYSRETURN(c) { f->eax = (c); return; }


void extract_params(struct intr_frame * f, const char * format, ...);

/* syscall_halt.c */
void halt_handler(struct intr_frame *f);

/* syscall_io.c */
void create_handler(struct intr_frame * f);
void remove_handler(struct intr_frame * f);
void open_handler(struct intr_frame * f);
void close_handler(struct intr_frame * f);
void write_handler(struct intr_frame * f);
void read_handler(struct intr_frame * f);
void seek_handler(struct intr_frame * f);
void tell_handler(struct intr_frame * f);
void filesize_handler(struct intr_frame * f);

/* syscall_exit.c */
void exit_handler(struct intr_frame * f);

/* syscall_proc.c */
void exec_handler(struct intr_frame * f);
void wait_handler(struct intr_frame * f);

#endif
