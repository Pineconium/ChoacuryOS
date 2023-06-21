#ifndef __PL011_H
#define __PL011_H

#include <stddef.h>
#include <stdint.h>

/* The ARM PrimeCell UART (PL011) is documented at
 *
 * http://infocenter.arm.com/help/topic/com.arm.doc.ddi0183g/index.html
 * 
 * In particular, Section 3 explains the programmer's model, i.e., how to 
 * interact with it: this includes 
 * 
 * - Section 3.2, which summarises the device register layout in Table 3.1
 *   (including an offset from the device base address, in the memory map,
 *   for each register), and
 * - Section 3.3, which summarises the internal structure of each device
 *   register.
 * 
 * Note that the field identifiers used here follow the documentation in a
 * general sense, but with a some minor alterations to improve clarity and
 * consistency.
 */

#define RSVD(x,y,z) uint8_t reserved##x[ z - y + 1 ];

typedef volatile struct {
  uint32_t    DR;            // base+0x0000          : data
union {
  uint32_t   RSR;            // base+0x0004          : receive status
  uint32_t   ECR;            //                      | error   clear
} ;
  RSVD( 0, 0x0008, 0x0017 ); // base+0x0008...0x0017 : reserved
  uint32_t    FR;            // base+0x0018          : flag
  RSVD( 1, 0x001C, 0x001F ); // base+0x001C...0x001F : reserved
  uint32_t   LPR;            // base+0x0020          : low-power counter
  uint32_t  IBRD;            // base+0x0024          : integer    baud rate
  uint32_t  FBRD;            // base+0x0028          : fractional baud rate
  uint32_t   LCR;            // base+0x002C          : line control 
  uint32_t    CR;            // base+0x0030          :      control
  uint32_t  IFLS;            // base+0x0034          : interrupt level select 
  uint32_t  IMSC;            // base+0x0038          : interrupt mask
  uint32_t   RIS;            // base+0x003C          : raw    interrupt status
  uint32_t   MIS;            // base+0x0040          : masked interrupt status
  uint32_t   ICR;            // base+0x0044          : interrupt clear
  uint32_t DMACR;            // base+0x0048          : DMA control
  RSVD( 2, 0x004C, 0x0FDF ); // base+0x004C...0x0FDF : reserved
  uint32_t PeriphID[ 4 ];    // base+0x0FE0...0x0FEC : peripheral ID
  uint32_t  PCellID[ 4 ];    // base+0x0FF0...0x0FFC : PrimeCell  ID
} PL011_t;

/* We know from
 * 
 * http://infocenter.arm.com/help/topic/com.arm.doc.dui0417d/index.html
 * 
 * that the platform has 4 PL011 devices(s), each of which we capture as an
 * instance of the PL011_t structure.
 */

extern PL011_t* const UART0;
extern PL011_t* const UART1;
extern PL011_t* const UART2;
extern PL011_t* const UART3;

// transmit raw      byte x via PL011 instance d
void    PL011_putc( PL011_t* d, uint8_t x );
// recieve  raw      byte x via PL011 instance d
uint8_t PL011_getc( PL011_t* d            );

// transmit hexified byte x via PL011 instance d
void    PL011_puth( PL011_t* d, uint8_t x );
// recieve  hexified byte x via PL011 instance d
uint8_t PL011_geth( PL011_t* d            );

#endif
