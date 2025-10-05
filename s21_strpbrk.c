#include "s21_string.h"

char *s21_strpbrk(const char *str1, const char *str2) {
  char *res = s21_NULL;
  char *s1 = (char *)str1;

  while (*s1 && !res) {
    char *s2 = (char *)str2;
    while (*s2 && !res) {
      if (*s1 == *s2) {
        res = s1;
      } else {
        s2++;
      }
    }
    s1++;
  }
  return res;
}