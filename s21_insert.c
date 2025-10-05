#include "s21_string.h"

void *s21_insert(const char *src, const char *str, size_t start_index) {
  char *res_str = s21_NULL;
  int offset = 0;
  if (src && str && start_index <= s21_strlen(src)) {
    res_str = malloc((s21_strlen(src) + s21_strlen(str) + 1) * sizeof(char));
  }
  for (s21_size_t i = 0; res_str && i < (s21_strlen(src) + 1); i++) {
    if ((i + offset) == start_index) {
      for (; offset < (int)(s21_strlen(str)); offset++) {
        res_str[i + offset] = str[offset];
      }
    }
    res_str[i + offset] = src[i];
  }
  return res_str;
}