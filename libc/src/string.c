#include <string.h>

int strlen(const char* str) {
  int len = 0;
  int i;
  for (i = 0; str[i] != 0; i++) {
    len++;
  }
  return len;
}