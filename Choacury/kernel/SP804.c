#include "SP804.h"

/* Per Table 4.2 (for example: the information is in several places) of
 * 
 * http://infocenter.arm.com/help/topic/com.arm.doc.dui0417d/index.html
 * 
 * we know the device(s) are mapped to fixed addresses in memory, so to
 * allow easier access to the device registers we can simply overlay an
 * associated structure instance for each one.
 */

SP804_t* const TIMER0 = ( SP804_t* )( 0x10011000 );
SP804_t* const TIMER1 = ( SP804_t* )( 0x10012000 );
SP804_t* const TIMER2 = ( SP804_t* )( 0x10018000 );
SP804_t* const TIMER3 = ( SP804_t* )( 0x10019000 );
