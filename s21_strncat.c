#include "s21_string.h"

char *s21_strncat(char *dest, const char *src, s21_size_t n) {
  s21_size_t dest_lenght = s21_strlen(dest);
  s21_size_t src_lenght = s21_strlen(src);
  if (n > src_lenght) {
    n = src_lenght;
  }
  for (s21_size_t i = 0; i < n; i++) {
    dest[dest_lenght + i] = src[i];
  }
  dest[dest_lenght + n] = '\0';
  return dest;
}