#include "common.h"

static void hang()
{
  for (;;)
    asm volatile("hlt");
}

void panic(const char *message)
{
  cls();
  kputs("kernel panic!\n");
  kputs(message);
  hang();
}
