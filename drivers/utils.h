#pragma once

#include "types.h"

void* memcpy(void *dest, const void *source, size_t nbytes);
void* memmove(void *dest, const void *source, size_t nbytes);
void* memset(void *dest, int val, size_t len);
int memcmp(const void *a, const void *b, size_t len);

typedef struct {
    bool valid;
    int value;
} atoi_result_t;
atoi_result_t atoi(const char*);

int abs(int x);

void uint64_to_string(uint64_t number, char* buffer);
int strlen(const char *str);
int strcmp(const char *str1, const char *str2);
char* strcpy(char *dest, const char *src);
char* strcat(char *dest, const char *src);

int tolower(int);
int toupper(int);
