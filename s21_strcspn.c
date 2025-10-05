#include "s21_string.h"

s21_size_t s21_strcspn(const char *str1, const char *str2) {
  s21_size_t res = 0;
  s21_size_t str2_lenght = s21_strlen(str2);
  s21_size_t i = 0;
  while (str1[res] != '\0' && str1[res] != str2[i]) {
    for (i = 0; i < str2_lenght; i++) {
      if (str1[res] == str2[i]) {
        break;
      }
    }
    if (str1[res] != str2[i]) {
      res++;
    }
  }
  return res;
}