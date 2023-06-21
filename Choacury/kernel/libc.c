#include "libc.h"

// Script containing functions for processes to use

int write( int fd, void* x, size_t n ) {
  int r;

  asm volatile( "mov r0, %1 \n"
                "mov r1, %2 \n"
                "mov r2, %3 \n"
                "svc #1     \n"
                "mov %0, r0 \n"
              : "=r" (r)
              : "r" (fd), "r" (x), "r" (n)
              : "r0", "r1", "r2" );

  return r;
}

void writeStr (char* str) {
    char buffer[50];
    strcpy(buffer, str);
    write(0, buffer, strlen(buffer));
    return;
}


void fork() {
    asm volatile("svc #2     \n");
    return;
}

void exitP() {
    asm volatile("svc #3     \n");
    return;
}

int eat(){
    int r;
    asm volatile(   "svc #4     \n"
                    "mov %0, r0 \n"
                :   "=r" (r));
    return r;
}

void putDown() {
    asm volatile("svc #5     \n");
    return;
}

char* itoa(int i, char b[]){
    char const digit[] = "0123456789";
    char* p = b;
    if(i<0){
        *p++ = '-';
        i *= -1;
    }
    int shifter = i;
    do{ //Move to where representation ends
        ++p;
        shifter = shifter/10;
    }while(shifter);
    *p = '\0';
    do{ //Move back, inserting digits as u go
        *--p = digit[i%10];
        i = i/10;
    }while(i);
    return b;
}
