#include "s21_string.h"

void *s21_memset(void *str, int c, s21_size_t n) {
  unsigned char *ptr_to_str = str;
  for (s21_size_t i = 0; i < n; i++) {
    ptr_to_str[i] = c;
  }
  return str;
}