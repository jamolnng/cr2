#include <string.h>

int memcmp(const void* aptr, const void* bptr, size_t size) {
  const unsigned char* a = (const unsigned char*)aptr;
  const unsigned char* b = (const unsigned char*)bptr;
  for (size_t i = 0; i < size; i++) {
    if (a[i] < b[i]) {
      return -1;
    } else if (b[i] < a[i]) {
      return 1;
    }
  }
  return 0;
}

void* memcpy(void* restrict dstptr, const void* restrict srcptr, size_t size) {
  unsigned char* dst = (unsigned char*)dstptr;
  const unsigned char* src = (const unsigned char*)srcptr;
  for (size_t i = 0; i < size; i++) {
    dst[i] = src[i];
  }
  return dstptr;
}

void* memmove(void* dstptr, const void* srcptr, size_t size) {
  unsigned char* dst = (unsigned char*)dstptr;
  const unsigned char* src = (const unsigned char*)srcptr;
  if (dst < src) {
    for (size_t i = 0; i < size; i++) {
      dst[i] = src[i];
    }
  } else {
    for (size_t i = size; i != 0; i--) {
      dst[i - 1] = src[i - 1];
    }
  }
  return dstptr;
}

void* memset(void* bufptr, int value, size_t size) {
  unsigned char* buf = (unsigned char*)bufptr;
  for (size_t i = 0; i < size; i++) {
    buf[i] = (unsigned char)value;
  }
  return bufptr;
}

int strcmp(const char* str1, const char* str2) {
  while (*str1) {
    if (*str1 != *str2) {
      break;
    }
    str1++;
    str2++;
  }
  return *(const unsigned char*)str1 - *(const unsigned char*)str2;
}

int strlen(const char* s) {
  int i;
  i = 0;
  while (s[i]) {
    i += 1;
  }
  return i;
}

char* strrev(char* str) {
  int i;
  int j;
  char a;
  int len = strlen((const char*)str);
  for (i = 0, j = len - 1; i < j; i++, j--) {
    a = str[i];
    str[i] = str[j];
    str[j] = a;
  }
  return str;
}