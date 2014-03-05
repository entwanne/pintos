#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include <stdarg.h>
#include "threads/thread.h"
#include "threads/vaddr.h"
#include "filesys/filesys.h"
#include "userprog/syscall_handlers.h"
#include "userprog/pagedir.h"

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

#define CRASH { \
      thread_current()->exit_status = -1; \
      thread_exit(); \
}

#define CHECK_ESP(esp) \
  if (esp == NULL || !is_user_vaddr(esp) || pagedir_get_page(thread_current()->pagedir, esp) == NULL) \
    CRASH;

static void check_ptr(const char *p, size_t size) {
  if (p == NULL) CRASH;
  while(size > 0) {
    if (!is_user_vaddr(p))
      CRASH;
    if (pagedir_get_page(thread_current()->pagedir, p) == NULL)
      CRASH;
    ++p;
    --size;
  }
}

static void check_str(const char *s) {
  if (s == NULL) CRASH;
  do {
    if (!is_user_vaddr(s))
      CRASH;
    if (pagedir_get_page(thread_current()->pagedir, s) == NULL)
      CRASH;
    ++s;
  } while(*s);
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
	  CHECK_ESP(esp);
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
  remove_handler, /* SYS_REMOVE */
  open_handler, /* SYS_OPEN */
  filesize_handler, /* SYS_FILESIZE */
  read_handler, /* SYS_READ */
  write_handler, /* SYS_WRITE */
  seek_handler, /* SYS_SEEK */
  tell_handler, /* SYS_TELL */
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
	CHECK_ESP(f->esp);
  unsigned int syscall = *((unsigned int*)f->esp);
  if (syscall < nb_handlers && handlers[syscall])
    handlers[syscall](f);
  else {
    printf ("system call %u!\n", syscall);
    thread_exit ();
  }
}
