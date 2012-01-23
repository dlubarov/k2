#include "handlers.h"
#include "common.h"
#include "isr.h"

// 0: division by zero exception
void handle_div0(registers_t *junk)
{
  panic("division by zero!");
}

// 8: double fault
void handle_double_fault(registers_t *regs)
{
  panic("double fault");
}

// 10: bad TSS
void handle_bad_tss(registers_t *regs)
{
  panic("bad TSS");
}

// 11: segment not present
void handle_seg_notpresent(registers_t *regs)
{
  panic("segmentation not present");
}

// 12: stack fault
void handle_stack_fault(registers_t *regs)
{
  panic("stack fault");
}

// 13: general protection fault
void handle_gen_protection_fault(registers_t *regs)
{
  panic("general protection fault");
}

// 14: page fault
void handle_page_fault(registers_t *regs)
{
  panic("page fault");
}

// 15: unknown interrupt exception
void handle_unknown_int(registers_t *junk)
{
  panic("unknown interrupt exception");
}

// 16: coprocessor fault
void handle_coproc_fault(registers_t *junk)
{
  panic("coprocessor fault");
}

// Register some generic interrupt handlers
void register_basic_handlers()
{
  register_interrupt_handler(0,  &handle_div0);
  register_interrupt_handler(8,  &handle_double_fault);
  register_interrupt_handler(10, &handle_bad_tss);
  register_interrupt_handler(11, &handle_seg_notpresent);
  register_interrupt_handler(12, &handle_stack_fault);
  register_interrupt_handler(13, &handle_gen_protection_fault);
  register_interrupt_handler(14, &handle_page_fault);
  register_interrupt_handler(15, &handle_unknown_int);
  register_interrupt_handler(16, &handle_coproc_fault);
}
