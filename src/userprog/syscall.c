#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/init.h"

static void syscall_handler (struct intr_frame *);

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}


static void halt_handler(struct intr_frame *f UNUSED)
{
  power_off();
}

typedef void(*handler_t)(struct intr_frame*);

static handler_t handlers[] = {
  halt_handler, /* SYS_HALT */
  NULL, /* SYS_EXIT */
  NULL, /* SYS_EXEC */
  NULL, /* SYS_WAIT */
  NULL, /* SYS_CREATE */
  NULL, /* SYS_REMOVE */
  NULL, /* SYS_OPEN */
  NULL, /* SYS_FILESIZE */
  NULL, /* SYS_READ */
  NULL, /* SYS_WRITE */
  NULL, /* SYS_SEEK */
  NULL, /* SYS_TELL */
  NULL, /* SYS_CLOSE */
  NULL, /* SYS_MMAP */
  NULL, /* SYS_MUNMAP */
  NULL, /* SYS_CHDIR */
  NULL, /* SYS_MKDIR */
  NULL, /* SYS_READDIR */
  NULL, /* SYS_ISDIR */
  NULL /* SYS_INUMBER */
};
static size_t nb_handlers = sizeof(handlers) / sizeof(handler_t);

static void
syscall_handler (struct intr_frame *f) 
{
  unsigned int syscall = *((unsigned int*)f->esp);
  if (syscall < nb_handlers && handlers[syscall])
    handlers[syscall](f);
  else {
    printf ("system call %u!\n", syscall);
    thread_exit ();
  }
}
