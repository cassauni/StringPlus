#include "s21_string.h"

int s21_memcmp(const void *str1, const void *str2, s21_size_t n) {
  unsigned char *s1 = (unsigned char *)str1;
  unsigned char *s2 = (unsigned char *)str2;
  int res = 0;
  for (s21_size_t i = 0; i < n && !res; i++) {
    res = s1[i] - s2[i];
  }
  return res;
}