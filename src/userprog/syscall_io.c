#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include <stdarg.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "threads/init.h"
#include "threads/vaddr.h"
#include "filesys/filesys.h"
#include "kernel/stdio.h"
#include "userprog/process.h"

#define SYSRETURN(c) { f->eax = c; return; }

void extract_params(struct intr_frame * f, const char * format, ...);

void write_handler(struct intr_frame * f) {
  int _fd;
  void * _buf;
  size_t _size;
  extract_params(f, "ipu", &_fd, &_buf, &_size);

  if (_fd == STDIN_FILENO) SYSRETURN(-1) // No write to stdin
  if (_fd == STDOUT_FILENO) {
    putbuf((char const *)_buf, _size);
    SYSRETURN(_size);
  } else {
    // TODO
    SYSRETURN(-1);
  }
}

void open_handler(struct intr_frame * f) {
  char const * _fname;
  extract_params(f, "p", &_fname);

  if (_fname == NULL) SYSRETURN(-1);

  int _fd = 0;
  struct thread * thr = thread_current();

  while (thr->fds[_fd] && _fd < MAX_FDS)
    ++_fd;
  if (_fd == MAX_FDS) SYSRETURN(-1);

  struct file * _file;
  _file = filesys_open(_fname);
  if (_file == NULL)
    SYSRETURN(-1);

  thr->fds[_fd] = (void *)_file;
  SYSRETURN(_fd + 2);
}
