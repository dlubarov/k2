// much code stolen from https://raw.github.com/torvalds/linux/master/drivers/net/ethernet/realtek/8139too.c

#include "rtl8139.h"
#include "../common.h"
#include "../pci.h"

#if 0
/* Symbolic offsets to registers. */
enum RTL8139_registers {
  MAC0		= 0,	 /* Ethernet hardware address. */
  MAR0		= 8,	 /* Multicast filter. */
  TxStatus0	= 0x10,	 /* Transmit status (Four 32bit registers). */
  TxAddr0		= 0x20,	 /* Tx descriptors (also four 32bit). */
  RxBuf		= 0x30,
  ChipCmd		= 0x37,
  RxBufPtr	= 0x38,
  RxBufAddr	= 0x3A,
  IntrMask	= 0x3C,
  IntrStatus	= 0x3E,
  TxConfig	= 0x40,
  RxConfig	= 0x44,
  Timer		= 0x48,	 /* A general-purpose counter. */
  RxMissed	= 0x4C,  /* 24 bits valid, write clears. */
  Cfg9346		= 0x50,
  Config0		= 0x51,
  Config1		= 0x52,
  TimerInt	= 0x54,
  MediaStatus	= 0x58,
  Config3		= 0x59,
  Config4		= 0x5A,	 /* absent on RTL-8139A */
  HltClk		= 0x5B,
  MultiIntr	= 0x5C,
  TxSummary	= 0x60,
  BasicModeCtrl	= 0x62,
  BasicModeStatus	= 0x64,
  NWayAdvert	= 0x66,
  NWayLPAR	= 0x68,
  NWayExpansion	= 0x6A,
  /* Undocumented registers, but required for proper operation. */
  FIFOTMS		= 0x70,	 /* FIFO Control and test. */
  CSCR		= 0x74,	 /* Chip Status and Configuration Register. */
  PARA78		= 0x78,
  FlashReg	= 0xD4,	/* Communication with Flash ROM, four bytes. */
  PARA7c		= 0x7c,	 /* Magic transceiver parameter register. */
  Config5		= 0xD8,	 /* absent on RTL-8139A */
};

enum ChipCmdBits {
  CmdReset	= 0x10,
  CmdRxEnb	= 0x08,
  CmdTxEnb	= 0x04,
  RxBufEmpty	= 0x01,
};

enum TxStatusBits {
  TxHostOwns	= 0x2000,
  TxUnderrun	= 0x4000,
  TxStatOK	= 0x8000,
  TxOutOfWindow	= 0x20000000,
  TxAborted	= 0x40000000,
  TxCarrierLost	= 0x80000000,
};

enum RxStatusBits {
  RxMulticast	= 0x8000,
  RxPhysical	= 0x4000,
  RxBroadcast	= 0x2000,
  RxBadSymbol	= 0x0020,
  RxRunt		= 0x0010,
  RxTooLong	= 0x0008,
  RxCRCErr	= 0x0004,
  RxBadAlign	= 0x0002,
  RxStatusOK	= 0x0001,
};

/* Bits in RxConfig. */
enum rx_mode_bits {
  AcceptErr	= 0x20,
  AcceptRunt	= 0x10,
  AcceptBroadcast	= 0x08,
  AcceptMulticast	= 0x04,
  AcceptMyPhys	= 0x02,
  AcceptAllPhys	= 0x01,
};

/* Bits in TxConfig. */
enum tx_config_bits {
  /* Interframe Gap Time. Only TxIFG96 doesn't violate IEEE 802.3 */
  TxIFGShift	= 24,
  TxIFG84		= (0 << TxIFGShift), /* 8.4us / 840ns (10 / 100Mbps) */
  TxIFG88		= (1 << TxIFGShift), /* 8.8us / 880ns (10 / 100Mbps) */
  TxIFG92		= (2 << TxIFGShift), /* 9.2us / 920ns (10 / 100Mbps) */
  TxIFG96		= (3 << TxIFGShift), /* 9.6us / 960ns (10 / 100Mbps) */

  TxLoopBack	= (1 << 18) | (1 << 17), /* enable loopback test mode */
  TxCRC		= (1 << 16),	/* DISABLE Tx pkt CRC append */
  TxClearAbt	= (1 << 0),	/* Clear abort (WO) */
  TxDMAShift	= 8, /* DMA burst value (0-7) is shifted X many bits */
  TxRetryShift	= 4, /* TXRR value (0-15) is shifted X many bits */

  TxVersionMask	= 0x7C800000, /* mask out version bits 30-26, 23 */
};

/* Bits in Config1 */
enum Config1Bits {
  Cfg1_PM_Enable	= 0x01,
  Cfg1_VPD_Enable	= 0x02,
  Cfg1_PIO	= 0x04,
  Cfg1_MMIO	= 0x08,
  LWAKE		= 0x10,		/* not on 8139, 8139A */
  Cfg1_Driver_Load = 0x20,
  Cfg1_LED0	= 0x40,
  Cfg1_LED1	= 0x80,
  SLEEP		= (1 << 1),	/* only on 8139, 8139A */
  PWRDN		= (1 << 0),	/* only on 8139, 8139A */
};

/* Bits in Config3 */
enum Config3Bits {
  Cfg3_FBtBEn   	= (1 << 0), /* 1	= Fast Back to Back */
  Cfg3_FuncRegEn	= (1 << 1), /* 1	= enable CardBus Function registers */
  Cfg3_CLKRUN_En	= (1 << 2), /* 1	= enable CLKRUN */
  Cfg3_CardB_En 	= (1 << 3), /* 1	= enable CardBus registers */
  Cfg3_LinkUp   	= (1 << 4), /* 1	= wake up on link up */
  Cfg3_Magic    	= (1 << 5), /* 1	= wake up on Magic Packet (tm) */
  Cfg3_PARM_En  	= (1 << 6), /* 0	= software can set twister parameters */
  Cfg3_GNTSel   	= (1 << 7), /* 1	= delay 1 clock from PCI GNT signal */
};

/* Bits in Config4 */
enum Config4Bits {
  LWPTN	= (1 << 2),	/* not on 8139, 8139A */
};

/* Bits in Config5 */
enum Config5Bits {
  Cfg5_PME_STS   	= (1 << 0), /* 1	= PCI reset resets PME_Status */
  Cfg5_LANWake   	= (1 << 1), /* 1	= enable LANWake signal */
  Cfg5_LDPS      	= (1 << 2), /* 0	= save power when link is down */
  Cfg5_FIFOAddrPtr= (1 << 3), /* Realtek internal SRAM testing */
  Cfg5_UWF        = (1 << 4), /* 1 = accept unicast wakeup frame */
  Cfg5_MWF        = (1 << 5), /* 1 = accept multicast wakeup frame */
  Cfg5_BWF        = (1 << 6), /* 1 = accept broadcast wakeup frame */
};

enum RxConfigBits {
  /* rx fifo threshold */
  RxCfgFIFOShift	= 13,
  RxCfgFIFONone	= (7 << RxCfgFIFOShift),

  /* Max DMA burst */
  RxCfgDMAShift	= 8,
  RxCfgDMAUnlimited = (7 << RxCfgDMAShift),

  /* rx ring buffer length */
  RxCfgRcv8K	= 0,
  RxCfgRcv16K	= (1 << 11),
  RxCfgRcv32K	= (1 << 12),
  RxCfgRcv64K	= (1 << 11) | (1 << 12),

  /* Disable packet wrap at end of Rx buffer. (not possible with 64k) */
  RxNoWrap	= (1 << 7),
};

/* Twister tuning parameters from RealTek.
   Completely undocumented, but required to tune bad links on some boards. */
enum CSCRBits {
  CSCR_LinkOKBit		= 0x0400,
  CSCR_LinkChangeBit	= 0x0800,
  CSCR_LinkStatusBits	= 0x0f000,
  CSCR_LinkDownOffCmd	= 0x003c0,
  CSCR_LinkDownCmd	= 0x0f3c0,
};

enum Cfg9346Bits {
  Cfg9346_Lock	= 0x00,
  Cfg9346_Unlock	= 0xC0,
};

typedef enum {
  CH_8139	= 0,
  CH_8139_K,
  CH_8139A,
  CH_8139A_G,
  CH_8139B,
  CH_8130,
  CH_8139C,
  CH_8100,
  CH_8100B_8139D,
  CH_8101,
} chip_t;

enum chip_flags {
  HasHltClk	= (1 << 0),
  HasLWake	= (1 << 1),
};

#define HW_REVID(b30, b29, b28, b27, b26, b23, b22) \
  (b30<<30 | b29<<29 | b28<<28 | b27<<27 | b26<<26 | b23<<23 | b22<<22)
#define HW_REVID_MASK	HW_REVID(1, 1, 1, 1, 1, 1, 1)

/* directly indexed by chip_t, above */
static const struct {
  const char *name;
  u32 version; /* from RTL8139C/RTL8139D docs */
  u32 flags;
} rtl_chip_info[] = {
  { "RTL-8139",
    HW_REVID(1, 0, 0, 0, 0, 0, 0),
    HasHltClk,
  },
  { "RTL-8139 rev K",
    HW_REVID(1, 1, 0, 0, 0, 0, 0),
    HasHltClk,
  },
  { "RTL-8139A",
    HW_REVID(1, 1, 1, 0, 0, 0, 0),
    HasHltClk, /* XXX undocumented? */
  },
  { "RTL-8139A rev G",
    HW_REVID(1, 1, 1, 0, 0, 1, 0),
    HasHltClk, /* XXX undocumented? */
  },
  { "RTL-8139B",
    HW_REVID(1, 1, 1, 1, 0, 0, 0),
    HasLWake,
  },
  { "RTL-8130",
    HW_REVID(1, 1, 1, 1, 1, 0, 0),
    HasLWake,
  },
  { "RTL-8139C",
    HW_REVID(1, 1, 1, 0, 1, 0, 0),
    HasLWake,
  },
  { "RTL-8100",
    HW_REVID(1, 1, 1, 1, 0, 1, 0),
    HasLWake,
  },
  { "RTL-8100B/8139D",
    HW_REVID(1, 1, 1, 0, 1, 0, 1),
    HasHltClk /* XXX undocumented? */
  | HasLWake,
  },
  { "RTL-8101",
  HW_REVID(1, 1, 1, 0, 1, 1, 1),
  HasLWake,
  },
};

struct rtl_extra_stats {
  unsigned long early_rx;
  unsigned long tx_buf_mapped;
  unsigned long tx_timeouts;
  unsigned long rx_lost_in_ring;
};

struct rtl8139_private {
  void __iomem		*mmio_addr;
  int			drv_flags;
  struct pci_dev		*pci_dev;
  u32			msg_enable;
  struct napi_struct	napi;
  struct net_device	*dev;

  unsigned char		*rx_ring;
  unsigned int		cur_rx;	/* RX buf index of next pkt */
  dma_addr_t		rx_ring_dma;

  unsigned int		tx_flag;
  unsigned long		cur_tx;
  unsigned long		dirty_tx;
  unsigned char		*tx_buf[NUM_TX_DESC];	/* Tx bounce buffers */
  unsigned char		*tx_bufs;	/* Tx bounce buffer region. */
  dma_addr_t		tx_bufs_dma;

  signed char		phys[4];	/* MII device addresses. */

        /* Twister tune state. */
  char			twistie, twist_row, twist_col;

  unsigned int		watchdog_fired : 1;
  unsigned int		default_port : 4; /* Last dev->if_port value. */
  unsigned int		have_thread : 1;

  spinlock_t		lock;
  spinlock_t		rx_lock;

  chip_t			chipset;
  u32			rx_config;
  struct rtl_extra_stats	xstats;

  struct delayed_work	thread;

  struct mii_if_info	mii;
  unsigned int		regs_len;
  unsigned long		fifo_copy_timeout;
};

/* write MMIO register, with flush */
/* Flush avoids rtl8139 bug w/ posted MMIO writes */
#define RTL_W8_F(reg, val8)	do { iowrite8 ((val8), ioaddr + (reg)); ioread8 (ioaddr + (reg)); } while (0)
#define RTL_W16_F(reg, val16)	do { iowrite16 ((val16), ioaddr + (reg)); ioread16 (ioaddr + (reg)); } while (0)
#define RTL_W32_F(reg, val32)	do { iowrite32 ((val32), ioaddr + (reg)); ioread32 (ioaddr + (reg)); } while (0)

/* write MMIO register */
#define RTL_W8(reg, val8)	iowrite8 ((val8), ioaddr + (reg))
#define RTL_W16(reg, val16)	iowrite16 ((val16), ioaddr + (reg))
#define RTL_W32(reg, val32)	iowrite32 ((val32), ioaddr + (reg))

/* read MMIO register */
#define RTL_R8(reg)		ioread8 (ioaddr + (reg))
#define RTL_R16(reg)		ioread16 (ioaddr + (reg))
#define RTL_R32(reg)		ioread32 (ioaddr + (reg))

static const u16 rtl8139_intr_mask =
  PCIErr | PCSTimeout | RxUnderrun | RxOverflow | RxFIFOOver |
  TxErr | TxOK | RxErr | RxOK;

static const u16 rtl8139_norx_intr_mask =
  PCIErr | PCSTimeout | RxUnderrun |
  TxErr | TxOK | RxErr ;

#if RX_BUF_IDX == 0
static const unsigned int rtl8139_rx_config =
  RxCfgRcv8K | RxNoWrap |
  (RX_FIFO_THRESH << RxCfgFIFOShift) |
  (RX_DMA_BURST << RxCfgDMAShift);
#elif RX_BUF_IDX == 1
static const unsigned int rtl8139_rx_config =
  RxCfgRcv16K | RxNoWrap |
  (RX_FIFO_THRESH << RxCfgFIFOShift) |
  (RX_DMA_BURST << RxCfgDMAShift);
#elif RX_BUF_IDX == 2
static const unsigned int rtl8139_rx_config =
  RxCfgRcv32K | RxNoWrap |
  (RX_FIFO_THRESH << RxCfgFIFOShift) |
  (RX_DMA_BURST << RxCfgDMAShift);
#elif RX_BUF_IDX == 3
static const unsigned int rtl8139_rx_config =
  RxCfgRcv64K |
  (RX_FIFO_THRESH << RxCfgFIFOShift) |
  (RX_DMA_BURST << RxCfgDMAShift);
#else
#error "Invalid configuration for 8139_RXBUF_IDX"
#endif

static const unsigned int rtl8139_tx_config =
	TxIFG96 | (TX_DMA_BURST << TxDMAShift) | (TX_RETRY << TxRetryShift);

#endif

void init_rtl8139(u16 bus, u16 slot, u16 func)
{
  kputs("Configuring the RTL8139 NIC");

  // Read the BARs.
  u32 io_bar = pci_config_readl(bus, slot, func, 16);
  u32 mem_bar = pci_config_readl(bus, slot, func, 20);
  ASSERT((io_bar & 1) == 1 && (mem_bar & 1) == 0, "io/mem confusion");

  // Read the base I/O port.
  u16 io_base_port = ((u16) io_bar) & 0xFFFC;

  // Read the base memory address.
  u8 mem_type = (mem_bar >> 1) & 3;
  ASSERT(mem_type == 0, "expected 32-bit memory space BAR for RTL8139");
  u32 mem_base_addr = mem_bar & 0xFFFFFFF0;
}
