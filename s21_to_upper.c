#include "s21_string.h"

void *s21_to_upper(const char *str) {
  char *new_str = s21_NULL;
  if (str) {
    new_str = malloc((s21_strlen(str) + 1) * sizeof(char));
  }
  if (new_str) {
    s21_strncpy(new_str, str, s21_strlen(str) + 1);
    for (s21_size_t i = 0; i < s21_strlen(new_str); i++) {
      if (new_str[i] >= 'a' && new_str[i] <= 'z') {
        new_str[i] += ('A' - 'a');
      }
    }
  }
  return new_str;
}