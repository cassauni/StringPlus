#include "s21_string.h"

char *s21_strstr(const char *haystack, const char *needle) {
  char *ind = s21_NULL;
  char *buff_f = (char *)needle;
  char *buff_s;
  for (; *haystack && ind == s21_NULL; haystack++) {
    buff_s = (char *)haystack;
    for (; *buff_s && *buff_f && *buff_f == *buff_s; buff_s++, buff_f++) {
    }
    if (!*buff_f) {
      ind = (char *)haystack;
    }
    buff_f = (char *)needle;
  }
  return ind;
}