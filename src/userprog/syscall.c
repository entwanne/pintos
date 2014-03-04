#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include <stdarg.h>
#include "threads/thread.h"
#include "threads/vaddr.h"
#include "filesys/filesys.h"
#include "userprog/syscall_handlers.h"

static void syscall_handler (struct intr_frame *);

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

#define POP_STACK_PARAM(esp, type, dest) {\
    dest = *((type*)(esp));		  \
    (esp) += sizeof(type);		  \
  }

#define ARG_PARAM(ap, type) *va_arg((ap), type*)

#define EXTRACT_PARAM(ap, esp, type) POP_STACK_PARAM(esp, type, ARG_PARAM(ap, type))

static void check_ptr(const char *p, size_t size) {
  if (p == NULL || !is_user_vaddr(p + size))
    {
      thread_current()->exit_status = -1;
      thread_exit();
    } // SEGV
}

static void check_str(const char *s) {
	check_ptr(s, 0);
}

void extract_params(struct intr_frame* f, const char* format, ...)
{
  va_list ap;
  va_start(ap, format);
  char *esp = (char*) f->esp + sizeof(int);
  char *str;
  void *buff;
  unsigned int size;
  while (format && *format) {
    switch (*format) {
    case 'i': // int
      EXTRACT_PARAM(ap, esp, int);
      break;
    case 'u': // unsigned int
      EXTRACT_PARAM(ap, esp, unsigned int);
      break;
    case 's': // string (char*)
      POP_STACK_PARAM(esp, char*, str);
      check_str(str);
      ARG_PARAM(ap, void*) = str;
      break;
    case 'b': // buffer (void* + size_t)
      POP_STACK_PARAM(esp, void*, buff);
      POP_STACK_PARAM(esp, size_t, size);
      check_ptr(buff, size);
      ARG_PARAM(ap, void*) = buff;
      ARG_PARAM(ap, size_t) = size;
      break;
    }
    format++;
  }
  va_end(ap);
}

typedef void(*handler_t)(struct intr_frame*);

static handler_t handlers[] = {
  halt_handler, /* SYS_HALT */
  exit_handler, /* SYS_EXIT */
  exec_handler, /* SYS_EXEC */
  wait_handler, /* SYS_WAIT */
  create_handler, /* SYS_CREATE */
  NULL, /* SYS_REMOVE */
  open_handler, /* SYS_OPEN */
  NULL, /* SYS_FILESIZE */
  read_handler, /* SYS_READ */
  write_handler, /* SYS_WRITE */
  NULL, /* SYS_SEEK */
  NULL, /* SYS_TELL */
  close_handler, /* SYS_CLOSE */
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
