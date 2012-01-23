#include "common.h"
#include "multiboot.h"
#include "dtables.h"
#include "pci.h"
#include "handlers.h"

#define CHECK_FLAG(flags,bit) ((flags) & (1 << (bit)))

void kmain(multiboot_info_t *mbi, unsigned int magic)
{
  if (magic != MULTIBOOT_BOOTLOADER_MAGIC)
    panic("bootloader did not supply multiboot magic number");

  if (CHECK_FLAG(mbi->flags, 4) && CHECK_FLAG(mbi->flags, 5))
    panic("bits 4 and 5 were both set");

  cls();
  kputs("Sup world?");

  if (CHECK_FLAG(mbi->flags, 6))
  {
    kputs("exploring memory map");
    memory_map_t *mmap;
    for (mmap = (memory_map_t *) mbi->mmap_addr;
         (unsigned long) mmap < mbi->mmap_addr + mbi->mmap_length;
         mmap = (memory_map_t *) ((unsigned long) mmap
                                  + mmap->size + sizeof (mmap->size)))
    {
      void *addr = (void *) mmap->base_addr_low;
      size_t len = mmap->length_low;
      if (addr == 0)
      {
        // We don't want malloc to return 0! Skip that byte.
        addr = (char *) addr + 4;
        len -= 4;
      }
      kprintf("    0x%x: 0x%x bytes\n", addr, len);
      raw_free(addr, len);
    }
  }
  else
    panic("no memory map was provided");

  init_descriptor_tables();

  register_basic_handlers();

  pci_scan();

  kputs("Goodbye");

  for (;;) ;
}
