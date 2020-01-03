#include <ctype.h>

int toupper(int ch) {
  if (ch >= 'a' && ch <= 'z') {
    ch = ch - ('a' - 'A');
  }
  return ch;
}

int tolower(int ch) {
  if (ch >= 'A' && ch <= 'Z') {
    ch = ch + ('a' - 'A');
  }
  return ch;
}