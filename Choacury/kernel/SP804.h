#ifndef __SP804_H
#define __SP804_H

#include <stddef.h>
#include <stdint.h>

/* The ARM Dual-Timer Module (SP804) is documented at
 * 
 * http://infocenter.arm.com/help/topic/com.arm.doc.ddi0271d/index.html
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
  uint32_t Timer1Load;       // base+0x0000          :            load
  uint32_t Timer1Value;      // base+0x0004          : current value
  uint32_t Timer1Ctrl;       // base+0x0008          : control
  uint32_t Timer1IntClr;     // base+0x000C          : interrupt clear
  uint32_t Timer1RIS;        // base+0x0010          : raw    interrupt status
  uint32_t Timer1MIS;        // base+0x0014          : masked interrupt status
  uint32_t Timer1BGLoad;     // base+0x0018          : background load
  RSVD( 0, 0x001C, 0x001F ); // base+0x001C...0x001F : reserved
  uint32_t Timer2Load;       // base+0x0020          :            load
  uint32_t Timer2Value;      // base+0x0024          : current value
  uint32_t Timer2Ctrl;       // base+0x0028          : control
  uint32_t Timer2IntClr;     // base+0x002C          : interrupt clear
  uint32_t Timer2RIS;        // base+0x0030          : raw    interrupt status
  uint32_t Timer2MIS;        // base+0x0034          : masked interrupt status
  uint32_t Timer2BGLoad;     // base+0x0038          : background load  
  RSVD( 1, 0x003C, 0x0EFF ); // base+0x003C...0x0EFF : reserved
  uint32_t TimerITCR;        // base+0x0F00          : integration test control
  uint32_t TimerITOP;        // base+0x0F04          : integration test control
  RSVD( 2, 0x0F08, 0x0FDF ); // base+0x0F08...0x0FDF : reserved
  uint32_t PeriphID[ 4 ];    // base+0x0FE0...0x0FEC : peripheral ID
  uint32_t  PCellID[ 4 ];    // base+0x0FF0...0x0FFC : PrimeCell  ID
} SP804_t;

/* We know from
 * 
 * http://infocenter.arm.com/help/topic/com.arm.doc.dui0417d/index.html
 * 
 * that the platform has 4 SP804 devices(s), each of which we capture as an 
 * instance of the SP804_t structure; note that each in fact supports *two* 
 * timers (each device is a *dual* timer module), so some care is needed to
 * translate each variable name, i.e., a timer module, to a given timer.
 */

extern SP804_t* const TIMER0; // timer module 0 -> timers #0 and #1
extern SP804_t* const TIMER1; // timer module 1 -> timers #2 and #3
extern SP804_t* const TIMER2; // timer module 2 -> timers #4 and #5
extern SP804_t* const TIMER3; // timer module 3 -> timers #6 and #7

#endif
