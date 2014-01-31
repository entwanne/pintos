#include "threads/init.h"
#include "userprog/syscall_handlers.h"

void halt_handler(struct intr_frame *f)
{
  (void) f;
  power_off();
}
