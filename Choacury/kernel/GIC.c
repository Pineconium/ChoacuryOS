#include "GIC.h"

/* Per Table 4.2 (for example: the information is in several places) of
 * 
 * http://infocenter.arm.com/help/topic/com.arm.doc.dui0417d/index.html
 * 
 * we know the device(s) are mapped to fixed addresses in memory, so to
 * allow easier access to the device registers we can simply overlay an
 * associated structure instance for each one.
*/

GICC_t* const GICC0 = ( GICC_t* )( 0x1E000000 );
GICD_t* const GICD0 = ( GICD_t* )( 0x1E001000 );
GICC_t* const GICC1 = ( GICC_t* )( 0x1E010000 );
GICD_t* const GICD1 = ( GICD_t* )( 0x1E011000 );
GICC_t* const GICC2 = ( GICC_t* )( 0x1E020000 );
GICD_t* const GICD2 = ( GICD_t* )( 0x1E021000 );
GICC_t* const GICC3 = ( GICC_t* )( 0x1E030000 );
GICD_t* const GICD3 = ( GICD_t* )( 0x1E031000 );
