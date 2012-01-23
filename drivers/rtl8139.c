#include "rtl8139.h"
#include "../common.h"
#include "../pci.h"

void init_rtl8139(u16 bus, u16 slot, u16 func)
{
  kputs("Configuring the RTL8139 NIC");

  // Read the BARs.
  u32 io_bar = pci_config_readl(bus, slot, func, 16);
  u32 mem_bar = pci_config_readl(bus, slot, func, 20);
  ASSERT((io_bar & 1) == 1 && (mem_bar & 1) == 0, "io/mem confusion");

  // Read the base I/O port.
  u16 io_port = ((u16) io_bar) & 0xFFFC;

  // Read the base memory address.
  u8 mem_type = (mem_bar >> 1) & 3;
  ASSERT(mem_type == 0, "expected 32-bit memory space BAR for RTL8139");
  u32 mem_addr = mem_bar & 0xFFFFFFF0;
}
