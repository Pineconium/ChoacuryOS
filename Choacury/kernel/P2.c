#include "P2.h"

// P2 - basic process that does arbitrary computing

uint32_t weight( uint32_t x ) {
  x = ( x & 0x55555555 ) + ( ( x >>  1 ) & 0x55555555 );
  x = ( x & 0x33333333 ) + ( ( x >>  2 ) & 0x33333333 );
  x = ( x & 0x0F0F0F0F ) + ( ( x >>  4 ) & 0x0F0F0F0F );
  x = ( x & 0x00FF00FF ) + ( ( x >>  8 ) & 0x00FF00FF );
  x = ( x & 0x0000FFFF ) + ( ( x >> 16 ) & 0x0000FFFF );

  return x;
}

void P2() {
    char* xx = "hello world, I'm P2\n";
    //write(0, xx, 20);
  while( 1 ) {

    // compute the Hamming weight of each x for 2^8 < x < 2^24

    for( uint32_t x = ( 1 << 8 ); x < ( 1 << 24 ); x++ ) {
        //write(0, xx, 20);
      uint32_t r = weight( x );  // printf( "weight( %d ) = %d\n", x, r );
      //printNum(x);
    }
  }

  return;
}

void (*entry_P2)() = &P2;
