#include "common.h"

// Write a byte out to the specified port.
void iowrite8(u16 port, u8 value)
{
  asm volatile("outb %1, %0" : : "dN" (port), "a" (value));
}

// Write two bytes out to the specified port.
void iowrite16(u16 port, u16 value)
{
  asm volatile("outw %1, %0" : : "dN" (port), "a" (value));
}

// Write four bytes out to the specified port.
void iowrite32(u16 port, u32 value)
{
  asm volatile("outl %1, %0" : : "dN" (port), "a" (value));
}

// Read a byte from the specified port.
u8 ioread8(u16 port)
{
  u8 ret;
  asm volatile("inb %1, %0" : "=a" (ret) : "dN" (port));
  return ret;
}

// Read two bytes from the specified port.
u16 ioread16(u16 port)
{
  u16 ret;
  asm volatile("inw %1, %0" : "=a" (ret) : "dN" (port));
  return ret;
}

// Read four bytes from the specified port.
u32 ioread32(u16 port)
{
  u32 ret;
  asm volatile("inl %1, %0" : "=a" (ret) : "dN" (port));
  return ret;
}
