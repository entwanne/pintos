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

static void
syscall_handler (struct intr_frame *f) 
{
  int syscall = *((int*)f->esp);
  switch (syscall)
    {
    case SYS_HALT:
      power_off();
      break;
    default:
      printf ("system call %d!\n", syscall);
      thread_exit ();
    }
}
