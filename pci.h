#pragma once

unsigned short pciConfigReadWord(unsigned short bus, unsigned short slot,
                                 unsigned short func, unsigned short offset);

void pci_scan();
