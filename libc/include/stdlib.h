#ifndef __CR2_LIBC_STDLIB_H__
#define __CR2_LIBC_STDLIB_H__
#include <stddef.h>

int ultoa(unsigned long num, char* str, int len, unsigned long base);
int itoa(int num, char* str, int len, int base);

#endif