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
#include "userprog/syscall_handlers.h"

void create_handler(struct intr_frame *f)
{
  char* filename;
  unsigned int size;
  extract_params(f, "su", &filename, &size);
  filesys_create(filename, size);
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

  thr->fds[_fd] = _file;
  SYSRETURN(_fd + 2);
}

void close_handler(struct intr_frame * f) {
  int _fd;
  extract_params(f, "i", &_fd);

  _fd -= 2;
  if (_fd < 0 || _fd > MAX_FDS) SYSRETURN(-1);

  struct thread * _thr = thread_current();
  struct file * _file = _thr->fds[_fd];
  if (_file == NULL) SYSRETURN(-1);

  file_close(_file);
  SYSRETURN(0);
}

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
    _fd -= 2;
    if (_fd < 0 || _fd > MAX_FDS) SYSRETURN(-1);

    struct thread * _thr = thread_current();
    struct file * _file = _thr->fds[_fd];
    if (_file == NULL) SYSRETURN(-1);

    _size = file_write(_file, _buf, _size);
    SYSRETURN(_size);
  }
}
