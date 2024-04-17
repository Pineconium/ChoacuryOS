#pragma once

#include "types.h"

void memory_copy(u8 *dest, const u8 *source, u32 nbytes);
void memory_move(u8 *dest, const u8 *source, u32 nbytes);
void memory_set(u8 *dest, u8 val, u32 len);
void int_to_ascii(int n, char str[]);

int strlen(const char *str);
int strcmp(const char *str1, const char *str2);
