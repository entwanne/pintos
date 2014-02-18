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
#include "devices/input.h"
#include "filesys/file.h"
#include "userprog/process.h"
#include "userprog/syscall_handlers.h"

void create_handler(struct intr_frame *f)
{
  char const* filename;
  unsigned int size;
  extract_params(f, "su", &filename, &size);
  bool ret = filesys_create(filename, size);
  SYSRETURN(!ret);
}

void open_handler(struct intr_frame * f) {
  char const * _fname;
  extract_params(f, "s", &_fname);

  if (_fname == NULL) SYSRETURN(-1);

  struct thread * thr = thread_current();
  int _fd = thr->low_fd;

  while (thr->fds[_fd] && _fd < MAX_FDS)
    ++_fd;
  if (_fd == MAX_FDS) SYSRETURN(-1);

  struct file * _file;
  _file = filesys_open(_fname);
  if (_file == NULL)
    SYSRETURN(-1);

  thr->fds[_fd] = _file;
  thr->low_fd++;
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

  if (_thr->low_fd > _fd)
    _thr->low_fd = _fd;

  file_close(_file);
  _thr->fds[_fd] = NULL;
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

void read_handler(struct intr_frame *f) {
  int _fd;
  void * _buf;
  size_t _size;
  extract_params(f, "ipu", &_fd, &_buf, &_size);

  if (_fd < 0 || _fd >= MAX_FDS) SYSRETURN(-1);
  if (_fd == STDOUT_FILENO) SYSRETURN(-1); // No read from stdout
  if (_fd == STDIN_FILENO) {
    uint8_t c;
    size_t i;
    for (i = 0; i < _size; ++i) {
      c = input_getc();
      ((uint8_t*) _buf)[i] = c;
    }
    SYSRETURN(i);
  } else {
    struct thread * thr = thread_current();
    struct file * _file;
    _fd -= 2;
    _file = thr->fds[_fd];
    if (_file == NULL) SYSRETURN(-1);
    _size = file_read(_file, _buf, _size);
    SYSRETURN(_size);
  }
}
