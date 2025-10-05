#include "s21_string.h"

void s21_check_null(char *str, char c, char **last, int *flag) {
  if (c == 0)
    str = s21_NULL;
  else
    *(str - 1) = 0;

  *last = str;
  *flag = 0;
}

char *s21_strtok(char *str, const char *delim) {
  static char *last;
  char *tok = s21_NULL;
  char *buff_del = (char *)delim;
  int flag = 1;
  int c = 0, sc;
  if (!(str == s21_NULL && (str = last) == s21_NULL)) {
    c = *str++;
  }
  for (; (sc = *buff_del) != 0 && c;) {
    if (sc == c) {
      c = *str++;
      buff_del = (char *)delim;
    } else {
      buff_del++;
    }
  }
  if (c == 0) {
    flag = 0;
    last = s21_NULL;
  }
  if (flag) {
    tok = str - 1;
  }
  for (; flag;) {
    c = *str++;
    buff_del = (char *)delim;
    do {
      if ((sc = *buff_del++) == c) {
        s21_check_null(str, c, &last, &flag);
      }
    } while (sc != 0 && flag);
  }

  return tok;
}