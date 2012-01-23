#include "pci.h"
#include "common.h"

// Read a word PCI configuration space.
u16 pci_config_readw(u32 bus, u32 slot, u32 func, u32 offset)
{
  u32 address;
//   u32 lbus = (u32) bus;
//   u32 lslot = (u32) slot;
//   u32 lfunc = (u32) func;

  // create configuration address
  address = (u32) ((bus << 16) | (slot << 11) | (func << 8) | (offset & 0xfc) | ((u32) 0x80000000));

  // write out the address
  outl(0xCF8, address);

  // read in the data
  // (offset & 2) * 8) = 0 will choose the fisrt word of the 32 bits register
  return (u16) ((inl(0xCFC) >> ((offset & 2) * 8)) & 0xffff);
}

// A very inefficient PCI scan.
void pci_scan()
{
  u16 bus, slot, func;
  for (bus = 0; bus < 256; ++bus)
    for (slot = 0; slot < 32; ++slot)
      for (func = 0; func < 8; ++func)
      {
        u16 vendorID = pci_config_readw(bus, slot, func, 0);
        u16 deviceID = pci_config_readw(bus, slot, func, 2);
        u8 empty = deviceID == 0xffff && vendorID == 0xffff;
        if (!empty)
          kprintf("Found vendor %d's device %d\n", vendorID, deviceID);
      }
}
