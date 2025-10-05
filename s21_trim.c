#include "s21_string.h"

void *s21_trim(const char *src, const char *trim_chars) {
  char *res_str = s21_NULL;
  if (src && trim_chars) {
    res_str = malloc((s21_strlen(src) + 1) * sizeof(char));
    *res_str = '\0';
  }
  if (res_str) {
    int offset = 0;
    while (*(src + offset) && s21_strchr(trim_chars, src[offset])) {
      offset++;
    }
    s21_strncpy(res_str, src + offset, s21_strlen(src + offset));
    res_str[s21_strlen(src + offset)] = '\0';
    while (*res_str &&
           s21_strchr(trim_chars, res_str[s21_strlen(res_str) - 1])) {
      res_str[s21_strlen(res_str) - 1] = '\0';
    }
  }

  return res_str;
}