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

int memcmp(const void *a, const void *b, u32 len) {
	const u8* a_u8 = (const u8*)a;
	const u8* b_u8 = (const u8*)b;
	for (u32 i = 0; i < len; i++) {
		if (a_u8[i] != b_u8[i]) {
			return (int)a_u8[i] - (int)b_u8[i];
		}
	}
	return 0;
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

atoi_result_t atoi(const char* str) {
	// parse optional sign
	bool negative = (*str == '-');
	if (*str == '-' || *str == '+') {
		str++;
	}

	// parse digits
	int value = 0;
	while (*str) {
		if (*str < '0' || *str > '9') {
			atoi_result_t result = { .valid = false, .value = 0 };
			return result;
		}
		value = (value * 10) + (*str - '0');
		str++;
	}

	if (negative) {
		value = -value;
	}

	atoi_result_t result = { .valid = true, .value = value };
	return result;
}

int strlen(const char *str) {
	int i = 0;
	while (str[i]) {
		i++;
	}
	return i;
}

int strcmp(const char *str1, const char *str2) {
	while (*str1 && *str2 && *str1 == *str2) {
		str1++;
		str2++;
	}
	return *str1 - *str2;
}

int strcpy(char *dest, const char *src) {
	int i = 0;
	while (src[i]) {
		dest[i] = src[i];
		i++;
	}
	dest[i] = '\0';
	return i;
}
