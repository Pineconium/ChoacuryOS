#include "utils.h"

void memory_copy(u8 *dest, const u8 *source, u32 nbytes) {
    for (u32 i = 0; i < nbytes; i++) {
        dest[i] = source[i];
    }
}

void memory_move(u8 *dest, const u8 *source, u32 nbytes) {
    if (dest < source) {
		for (u32 i = 0; i < nbytes; i++) {
			dest[i] = source[i];
        }
	} else {
		for (u32 i = 1; i <= nbytes; i++) {
			dest[nbytes - i] = source[nbytes - i];
        }
	}
}

void memory_set(u8 *dest, u8 val, u32 len) {
    for (; len != 0; len--) {
        *dest++ = val;
    }
}

void iota(int n, char str[]) {
    int i, sign;
    if ((sign = n) < 0) n = -n;
    i = 0;
    do {
        str[i++] = n % 10 + '0';
    } while ((n /= 10) > 0);

    if (sign < 0) str[i++] = '-';
    str[i] = '\0';
}