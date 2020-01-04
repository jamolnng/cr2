#include <stdlib.h>

#include <string.h>

#include <uart.h>

int ultoa(unsigned long num, char* str, int len, unsigned long base) {
  if (len == 0) return -1;
  int neg = 0;
  if (num < 0) {
    num = -num;
    neg = 1;
    len--;
  }
  unsigned long sum = num;
  unsigned long i = 0;
  unsigned long digit;
  do {
    digit = sum % base;
    if (digit < 0xA)
      str[i++] = '0' + digit;
    else
      str[i++] = 'A' + digit - 0xA;
    sum /= base;
  } while (sum && (i < (len - 1)));
  if (i == (len - 1) && sum) return -1;
  if (neg) {
    str[i++] = '-';
  }
  str[i] = '\0';
  strrev(str);
  return 0;
}

int itoa(int num, char* str, int len, int base) {
  if (len == 0) return -1;
  int neg = 0;
  if (num < 0) {
    num = -num;
    neg = 1;
    len--;
  }
  int sum = num;
  int i = 0;
  int digit;
  do {
    digit = sum % base;
    if (digit < 0xA)
      str[i++] = '0' + digit;
    else
      str[i++] = 'A' + digit - 0xA;
    sum /= base;
  } while (sum && (i < (len - 1)));
  if (i == (len - 1) && sum) return -1;
  if (neg) {
    str[i++] = '-';
  }
  str[i] = '\0';
  strrev(str);
  return 0;
}