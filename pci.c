#include "pci.h"
#include "common.h"

#include "drivers/rtl8139.h"

// Read four bytes from PCI config space.
u32 pci_config_readl(u32 bus, u32 slot, u32 func, u32 offset)
{
  ASSERT((offset & 3) == 0, "pci read not aligned");

  // create configuration address
  u32 address = (bus << 16) | (slot << 11) | (func << 8) | offset | (u32) 0x80000000;

  // write out the address
  outl(0xCF8, address);

  // read in the data
  return inl(0xCFC);
}

// Read two byes from PCI config space.
u16 pci_config_readw(u32 bus, u32 slot, u32 func, u32 offset)
{
  u32 ldata = pci_config_readl(bus, slot, func, offset & 0xfc);
  return (u16) (ldata >> (8 * offset));
}

// Read a byte from PCI config space.
u8 pci_config_readb(u32 bus, u32 slot, u32 func, u32 offset)
{
  u32 ldata = pci_config_readl(bus, slot, func, offset & 0xfc);
  return (u8) (ldata >> (8 * offset));
}

// A very inefficient PCI enumeration routine.
void pci_scan()
{
  u16 bus, slot, func;
  for (bus = 0; bus < 256; ++bus)
    for (slot = 0; slot < 32; ++slot)
      for (func = 0; func < 8; ++func)
      {
        u16 vendorID = pci_config_readw(bus, slot, func, 0);
        u16 deviceID = pci_config_readw(bus, slot, func, 2);
        u8 empty = vendorID == 0xffff;
        if (!empty)
        {
          kprintf("Found vendor 0x%x's device: 0x%x\n", vendorID, deviceID);

          int i;
          for (i = 0; i < 6; ++i)
          {
            u32 bar = pci_config_readl(bus, slot, func, 16 + 4*i);
            if (bar == 0)
              continue;
            u8 is_io = bar & 1;
            if (is_io)
            {
              kprintf("    IO BAR: 0x%x\n", bar >> 2 << 2);
            }
            else
              kprintf("    Memory BAR: 0x%x\n", bar >> 4 << 4);
          }

          if (vendorID == 0x10ec && deviceID == 0x8139)
          {
            init_rtl8139(bus, slot, func);
          }
        }
      }
}
