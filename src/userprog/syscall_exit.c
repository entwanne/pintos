#include "threads/init.h"
#include "userprog/syscall_handlers.h"

void exit_handler(struct intr_frame *f) {
  int _code;
  extract_params(f, "i", &_code);
  (void)_code;

  process_exit();
  thread_exit();
}
