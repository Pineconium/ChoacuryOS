#pragma once

#include "types.h"
#include <stdbool.h>

void memory_copy(u8 *dest, const u8 *source, u32 nbytes);
void memory_move(u8 *dest, const u8 *source, u32 nbytes);
void memory_set(u8 *dest, u8 val, u32 len);

typedef struct {
	bool valid;
	int value;
} atoi_result_t;
atoi_result_t atoi(const char*);

int strlen(const char *str);
int strcmp(const char *str1, const char *str2);
int strcpy(char *dest, const char *src);
