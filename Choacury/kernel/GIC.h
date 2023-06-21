#ifndef __GIC_H
#define __GIC_H

#include <stddef.h>
#include <stdint.h>

/* Although the GIC architecture is documented at
 * 
 * the platform includes a bespoke implementation based on the combination
 * of 4 GIC components in total.
 * 
 * - Section 3.14 gives a high-level overview of the interrupt mechanism,
 *   noting in particular that GIC0 and GIC1 are those associated with the
 *   ARM core (managing IRQ- and FIQ-based interrupts respectively),
 * - Section 4.11 describes the GIC implementation, which sub-divides each
 *   GIC into interface and distributor components: it includes 
 * 
 *   - Table 4.44,           i.e., the mapping of interrupt signals from 
 *     other devices to interrupt IDs wrt. each GIC,
 *   - Tables 4.46 and 4.55, i.e., the device register layout (including 
 *     an offset from the device base address, in the memory map, for each 
 *     register), plus
 *   - a summary of the internal structure of each device register.
 * 
 * Note that the field identifiers used here follow the documentation in a
 * general sense, but with a some minor alterations to improve clarity and
 * consistency.
 */

#define RSVD(x,y,z) uint8_t reserved##x[ z - y + 1 ];

typedef volatile struct {
  uint32_t       CTLR;       // base+0x0000          : control
  uint32_t        PMR;       // base+0x0004          : priority mask
  uint32_t        BPR;       // base+0x0008          : binary point
  uint32_t        IAR;       // base+0x000C          : interrupt acknowledge
  uint32_t       EOIR;       // base+0x0010          : end of interrupt
  uint32_t        RPR;       // base+0x0014          : running interrupt
  uint32_t      HPPIR;       // base+0x0018          : highest pending interrupt
} GICC_t;

typedef volatile struct {
  uint32_t       CTLR;       // base+0x0000          : control
  uint32_t      TYPER;       // base+0x0004          : controller type
  RSVD( 0, 0x0008, 0x00FC ); // base+0x0008...0x00FC : reserved
  uint32_t  ISENABLER[  3 ]; // base+0x0100...0x0108 :   set-enable
  RSVD( 1, 0x010C, 0x017C ); // base+0x010C...0x017C : reserved
  uint32_t  ICENABLER[  3 ]; // base+0x0180...0x0188 : clear-enable
  RSVD( 2, 0x018C, 0x01FC ); // base+0x018C...0x01FC : reserved
  uint32_t    ISPENDR[  3 ]; // base+0x0200...0x0208 :   set-pending
  RSVD( 3, 0x020C, 0x027C ); // base+0x020C...0x027C : reserved
  uint32_t    ICPENDR[  3 ]; // base+0x0280...0x0288 : clear-pending
  RSVD( 4, 0x028C, 0x02FC ); // base+0x028C...0x02FC : reserved
  uint32_t  ISACTIVER[  3 ]; // base+0x0300...0x0308 :   set-active
  RSVD( 5, 0x030C, 0x03FC ); // base+0x030C...0x03FC : reserved
  uint32_t IPRIORITYR[ 24 ]; // base+0x0400...0x045C : priority
  RSVD( 6, 0x0460, 0x07FC ); // base+0x0460...0x07FC : reserved
  uint32_t  ITARGETSR[ 24 ]; // base+0x0800...0x085C : processor target
  RSVD( 7, 0x0860, 0x0BFC ); // base+0x0760...0x0BFC : reserved
  uint32_t      ICFGR[  6 ]; // base+0x0C00...0x0C14 : configuration
  RSVD( 8, 0x0C18, 0x0EFC ); // base+0x0C18...0x0EFC : reserved
  uint32_t       SGIR;       // base+0x0F00          : software interrupt
  RSVD( 9, 0x0F04, 0x0FFC ); // base+0x0F04...0x0FFC : reserved
} GICD_t;

#define GIC_SOURCE_TIMER0 ( 36 )
#define GIC_SOURCE_TIMER1 ( 37 )
#define GIC_SOURCE_TIMER2 ( 73 )
#define GIC_SOURCE_TIMER3 ( 74 )

#define GIC_SOURCE_UART0  ( 44 )
#define GIC_SOURCE_UART1  ( 45 )
#define GIC_SOURCE_UART2  ( 46 )
#define GIC_SOURCE_UART3  ( 47 )

/* We know from
 * 
 * http://infocenter.arm.com/help/topic/com.arm.doc.dui0417d/index.html
 * 
 * that the platform has 4 GIC devices(s), each of which we capture as an
 * instance of the GICC_t and GICD_t structures.
 */

extern GICC_t* const GICC0;
extern GICD_t* const GICD0;
extern GICC_t* const GICC1;
extern GICD_t* const GICD1;
extern GICC_t* const GICC2;
extern GICD_t* const GICD2;
extern GICC_t* const GICC3;
extern GICD_t* const GICD3;

#endif
