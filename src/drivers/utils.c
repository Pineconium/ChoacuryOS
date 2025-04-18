#include "utils.h"
#include <stdint.h>

void* memcpy(void* dst, const void* src, size_t len) {
    u8* dst_u8 = (u8*)dst;
    const u8* src_u8 = (const u8*)src;
    for (size_t i = 0; i < len; i++) {
        dst_u8[i] = src_u8[i];
    }
    return dst;
}

void* memmove(void* dst, const void* src, size_t len) {
    u8* dst_u8 = (u8*)dst;
    const u8* src_u8 = (const u8*)src;
    if (dst_u8 < src_u8) {
        for (size_t i = 0; i < len; i++) {
            dst_u8[i] = src_u8[i];
        }
    } else {
        for (u32 i = 1; i <= len; i++) {
            dst_u8[len - i] = src_u8[len - i];
        }
    }
    return dst;
}

void* memset(void* dst, int val, size_t len) {
    u8* dst_u8 = (u8*)dst;
    for (size_t i = 0; i < len; i++) {
        dst_u8[i] = val;
    }
    return dst;
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

int abs(int x) {
	return x < 0 ? -x : x;
}

// VuleSuma
float fmod(float x, float y) {
    return x - (x / y) * y;
}

// Fixed point sin
float sin(float x) {
    x = fmod(x, 2 * M_PI);
    if (x < -M_PI) x += 2 * M_PI;
    if (x > M_PI) x -= 2 * M_PI;
    float term = x;
    float sinValue = term;
    for (int i = 1; i < 10; ++i) {
        term *= -x * x / ((2 * i) * (2 * i + 1));
        sinValue += term;
    }
    return sinValue;
}

// Fixed point cos
float cos(float x) {
    x = fmod(x, 2 * M_PI);
    if (x < -M_PI) x += 2 * M_PI;
    if (x > M_PI) x -= 2 * M_PI;

    float term = 1.0f;
    float cosValue = term;
    for (int i = 1; i < 10; ++i) {
        term *= -x * x / ((2 * i - 1) * (2 * i));
        cosValue += term;
    }
    return cosValue;
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

char* strcpy(char* dest, const char* src) {
    int i = 0;
    while (src[i]) {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
    return dest;
}

char* strcat(char* dest, const char* src) {
    int len = strlen(dest);
    int i = 0;
    while (src[i]) {
        dest[len + i] = src[i];
        i++;
    }
    dest[len + i] = '\0';
    return dest;
}

int tolower(int ch) {
	if (ch < 'A' || ch > 'Z') {
		return ch;
	}
	return ch - 'A' + 'a';
}

int toupper(int ch) {
	if (ch < 'a' || ch > 'z') {
		return ch;
	}
	return ch - 'a' + 'A';
}

/* This isn't an actual utility, it's just for uint64_to_string(). */
int get_num_length(uint64_t num) {
    int length = 0;
    do {
        length++;
        num /= 10;
    } while (num > 0);
    return length;
}

void uint64_to_string(uint64_t number, char* buffer) {
    int length = get_num_length(number);
    buffer[length] = '\0';
    int index = length - 1;
    do {
        buffer[index--] = '0' + (number % 10);
        number /= 10;
    } while (number > 0);
}
