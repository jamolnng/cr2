#ifndef __CR2_LIBC_STRING_H__
#define __CR2_LIBC_STRING_H__

#include <stddef.h>

int memcmp(const void*, const void*, size_t);
void* memcpy(void*, const void*, size_t);
void* memmove(void*, const void*, size_t);
void* memset(void*, int, size_t);
int strmcp(const char*, const char*);
int strlen(const char*);
char* strrev(char*);

#endif