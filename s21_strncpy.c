#include "s21_string.h"

char *s21_strncpy(char *dest, const char *src, s21_size_t n) {
  char *dst = dest;
  char *s = (char *)src;
  while (n != 0) {
    if ((*dst = *s) != 0) {
      dst++;
      s++;
    }
    n--;
  }
  return dest;
}