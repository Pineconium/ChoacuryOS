#include "PL011.h"

/* Per Table 4.2 (for example: the information is in several places) of
 * 
 * http://infocenter.arm.com/help/topic/com.arm.doc.dui0417d/index.html
 * 
 * we know the device(s) are mapped to fixed addresses in memory, so to
 * allow easier access to the device registers we can simply overlay an
 * associated structure instance for each one.
 */

PL011_t* const UART0 = ( PL011_t* )( 0x10009000 );
PL011_t* const UART1 = ( PL011_t* )( 0x1000A000 );
PL011_t* const UART2 = ( PL011_t* )( 0x1000B000 );
PL011_t* const UART3 = ( PL011_t* )( 0x1000C000 );

int  xtoi( char x ) {
  if      ( ( x >= '0' ) && ( x <= '9' ) ) {
    return (  0 + ( x - '0' ) );
  }
  else if ( ( x >= 'a' ) && ( x <= 'f' ) ) {
    return ( 10 + ( x - 'a' ) );
  }
  else if ( ( x >= 'A' ) && ( x <= 'F' ) ) {
    return ( 10 + ( x - 'A' ) );
  }

  return -1;
}

char itox( int  x ) {
  if     ( ( x >=  0 ) && ( x <=  9 ) ) {
    return '0' + ( x -  0 );
  }
  else if( ( x >= 10 ) && ( x <= 15 ) ) {
    return 'A' + ( x - 10 );
  }

  return -1;
}

void    PL011_putc( PL011_t* d, uint8_t x ) {
  while( d->FR & 0x20 ) {
    /* wait while transmit FIFO is full */
  }

  d->DR = x;
}

uint8_t PL011_getc( PL011_t* d            ) {
  while( d->FR & 0x10 ) {
    /* wait while receive FIFO is empty */
  }

  return d->DR;
}

void    PL011_puth( PL011_t* d, uint8_t x ) {
  PL011_putc( d, itox( ( x >> 4 ) & 0xF ) );
  PL011_putc( d, itox( ( x >> 0 ) & 0xF ) );
}

uint8_t PL011_geth( PL011_t* d            ) {
  uint8_t x  = ( xtoi( PL011_getc( d ) ) << 4 );
          x |= ( xtoi( PL011_getc( d ) ) << 0 );

  return x;
}
