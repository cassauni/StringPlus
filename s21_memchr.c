#include "s21_string.h"

void *s21_memchr(const void *str, int c, s21_size_t n) {
  void *res = s21_NULL;
  unsigned char *string = (unsigned char *)str;
  for (s21_size_t i = 0; i < n && !res; i++) {
    if (string[i] == c) {
      res = &string[i];
    }
  }
  return res;
}