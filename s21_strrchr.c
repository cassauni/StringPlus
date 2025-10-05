#include "s21_string.h"

char *s21_strrchr(const char *str, int c) {
  char *res = s21_NULL;
  s21_size_t lenght = s21_strlen(str);
  for (int i = lenght; i >= 0 && !res; i--) {
    if (str[i] == c) {
      res = ((char *)str) + i;
    }
  }
  return res;
}