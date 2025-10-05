#include "s21_sscanf.h"

int s21_sscanf(const char *str, const char *format, ...) {
  int var_cnt = 0;
  if (s21_strchr(format, '%') != s21_NULL) {
    va_list arg;
    va_start(arg, format);
    var_cnt = processing(str, format, arg);
    va_end(arg);
  }
  return var_cnt;
}
void process_cycle(const char *str, const char *format, va_list arg,
                   int *var_cnt, s21_size_t *str_p, s21_size_t *format_p,
                   int *err, int *run) {
  token tok = {-1, -1, -1, -1};
  if (format[*format_p] == '%') {
    (*format_p)++;
    s21_size_t width_len = 0;
    char width[256] = "\0";
    if (format[*format_p] == '*') {
      tok.star = 1;
      (*format_p)++;
    }
    while (format[*format_p] && is_digit(format[*format_p]))
      width[width_len++] = format[(*format_p)++];
    width[width_len] = '\0';
    if (width_len > 0)
      tok.width = (s21_size_t)s21_atoi(width, width_len);
    else
      tok.width = -1;
    if (format[(*format_p)] && is_length(format[(*format_p)]))
      tok.length = format[(*format_p)++];
    else
      tok.length = -1;
    if (*str_p > s21_strlen(str) || str[*str_p] == '\0') {
      if (format[*format_p] == 'n') {
        tok.spec = format[(*format_p)++];
        init_var(str_p, str, arg, tok, var_cnt, err, run);
      } else
        *run = 0;
    } else {
      if (format[(*format_p)] && is_spec(format[(*format_p)])) {
        tok.spec = format[(*format_p)++];
        init_var(str_p, str, arg, tok, var_cnt, err, run);
      } else
        *err = 1;
    }
  } else if (format[*format_p] && s21_isspace(format[*format_p])) {
    while (format[*format_p] && s21_isspace(format[*format_p])) (*format_p)++;
    while (str[*str_p] && s21_isspace(str[*str_p])) (*str_p)++;
  } else if (format[*format_p] == str[*str_p]) {
    (*format_p)++;
    (*str_p)++;
  } else
    *run = 0;
}

int processing(const char *str, const char *format, va_list arg) {
  int var_cnt = 0;
  if (str[0] == '\0')
    var_cnt = -1;
  else {
    s21_size_t str_p = 0, format_p = 0;
    int err = 0, run = 1;
    while (run && !err && format[format_p] != '\0') {
      process_cycle(str, format, arg, &var_cnt, &str_p, &format_p, &err, &run);
    }
  }
  return var_cnt;
}

int is_digit(char c) {
  char *digits = "0123456789";
  return s21_strchr(digits, c) != s21_NULL ? 1 : 0;
}

int is_oct(char c) {
  char *octs = "01234567";
  return s21_strchr(octs, c) != s21_NULL ? 1 : 0;
}

int is_hex(char c) {
  char *hexes = "0123456789ABCDEFabcdef";
  return s21_strchr(hexes, c) != s21_NULL ? 1 : 0;
}

int is_length(char c) { return c == 'l' || c == 'L' || c == 'h' ? 1 : 0; }

int is_spec(char c) {
  char *specifs = "cdieEfgGosuxXpn%%";
  return s21_strchr(specifs, c) != s21_NULL ? 1 : 0;
}

long long s21_atoi(const char *str, s21_size_t n) {
  long long res = 0;
  long int mul = 1;
  long long res_buf = res;
  s21_size_t i = 0;
  if (str[i] == '-' || str[i] == '+') {
    if (str[i] == '-') {
      mul = -1;
    }
    i++;
  }
  for (; i < n; i++) {
    res_buf = res;
    res *= 10;
    res += mul * (int)(str[i] - '0');
    if (res_buf > res && mul == 1) {
      res = 9223372036854775807;
      i = n;
    } else if (res_buf < res && mul == -1) {
      res = -9223372036854775808ULL;
      mul = 1;
      i = n;
    }
  }
  return res;
}

long long oct_to_dec(const char *str, s21_size_t n) {
  long long res = 0;
  int mul = 1;
  long long res_buf = res;
  s21_size_t i = 0;
  if (str[i] == '-' || str[i] == '+') {
    if (str[i] == '-') mul = -1;
    i++;
  }

  if (str[i] == '0') i++;

  for (; i < n && str[i] != '\0'; i++) {
    res_buf = res;
    res *= 8;
    res += mul * (int)(str[i] - '0');
    if (res_buf > res && mul == 1) {
      res = 9223372036854775807;
      i = n;
    } else if (res_buf < res && mul == -1) {
      res = -9223372036854775808ULL;
      mul = 1;
      i = n;
    }
  }

  return res;
}

long long hex_to_dec(const char *str, s21_size_t n) {
  long long res = 0;
  int mul = 1;
  long long res_buf = res;
  s21_size_t i = 0;
  if (str[i] == '-' || str[i] == '+') {
    if (str[i] == '-') mul = -1;
    i++;
  }

  for (; i < n && str[i] != '\0'; i++) {
    res_buf = res;
    res *= 16;
    if (is_digit(str[i]))
      res += mul * (int)(str[i] - '0');
    else {
      if ('A' <= str[i] && str[i] <= 'F')
        res += mul * (int)(str[i] - 'A' + 10);
      else if ('a' <= str[i] && str[i] <= 'f')
        res += mul * (int)(str[i] - 'a' + 10);
    }
    if (res_buf > res && mul == 1) {
      res = 9223372036854775807;
      i = n;
    } else if (res_buf < res && mul == -1) {
      res = -9223372036854775808ULL;
      mul = 1;
      i = n;
    }
  }
  return res;
}

long double s21_atold(const char *str, s21_size_t n) {
  long double res = 0;
  int mul = 1;
  s21_size_t i = 0;
  if (str[i] == '-' || str[i] == '+') {
    if (str[i] == '-') mul = -1;
    i++;
  }

  for (; i < n && str[i] != '.' && str[i] != 'e' && str[i] != '\0'; i++) {
    res *= 10;
    res += (int)(str[i] - '0');
  }

  if (i + 1 < n && str[i] == '.') {
    i++;
    long int after_dot = 0, precision = 0;
    for (; i < n && str[i] != 'e' && str[i] != '\0'; i++) {
      after_dot *= 10;
      after_dot += (int)(str[i] - '0');
      precision++;
    }
    res += (long double)(after_dot / (pow(10, precision)));
  }

  if (i + 1 < n && str[i] == 'e') {
    i++;
    int pow_e = 0;
    int mul_e = 1;
    if (str[i] == '-' || str[i] == '+') {
      if (str[i] == '-') mul_e = -1;
      i++;
    }
    for (; i < n && str[i] != '\0'; i++) {
      pow_e *= 10;
      pow_e += (int)(str[i] - '0');
    }
    res *= pow(10, pow_e * mul_e);
  }
  return (long double)(res * mul);
}

void init_var(s21_size_t *str_p, const char *str, va_list arg, token tok,
              int *var_cnt, int *err, int *run) {
  if (tok.spec == 'c')
    init_c(str_p, str, arg, tok, var_cnt, err);
  else if (tok.spec == 'd')
    init_dec(str_p, str, arg, tok, var_cnt, err, run, 1);
  else if (tok.spec == 'i')
    init_i(str_p, str, arg, tok, var_cnt, err);
  else if (tok.spec == 'e' || tok.spec == 'E' || tok.spec == 'f' ||
           tok.spec == 'g' || tok.spec == 'G')
    init_e(str_p, str, arg, tok, var_cnt, err);
  else if (tok.spec == 'o')
    init_o(str_p, str, arg, tok, var_cnt, err);
  else if (tok.spec == 's')
    init_s(str_p, str, arg, tok, var_cnt, err);
  else if (tok.spec == 'u')
    init_dec(str_p, str, arg, tok, var_cnt, err, run, 0);
  else if (tok.spec == 'x' || tok.spec == 'X')
    init_x(str_p, str, arg, tok, var_cnt, err);
  else if (tok.spec == 'p')
    init_p(str_p, str, arg, tok, var_cnt, err);
  else if (tok.spec == 'n')
    init_n(*str_p, arg, tok, err);
  else if (tok.spec == '%')
    init_percent(str_p, str, tok, err, run);
}

int s21_isspace(char c) {
  return s21_strchr(" \t\n\v\f\r", c) != s21_NULL ? 1 : 0;
}

void init_c(s21_size_t *str_p, const char *str, va_list arg, token tok,
            int *var_cnt, int *err) {
  if ((tok.width < 0 && tok.width != -1) ||
      (tok.length != 'l' && tok.length != -1))
    *err = 1;
  else {
    if (tok.star == -1 && str[*str_p] != '\0') {
      write_c(arg, tok, *str_p, str, var_cnt);
    }

    (*str_p)++;
  }
}

void write_c(va_list arg, token tok, int str_p, const char *str, int *var_cnt) {
  void *var = va_arg(arg, void *);

  if (tok.length == 'l')
    *((wchar_t *)var) = (wchar_t)str[str_p];
  else if (tok.length == -1)
    *((char *)var) = (char)str[str_p];

  (*var_cnt)++;
}

void init_s(s21_size_t *str_p, const char *str, va_list arg, token tok,
            int *var_cnt, int *err) {
  if ((tok.width <= 0 && tok.width != -1) ||
      (tok.length != -1 && tok.length != 'l'))
    *err = 1;
  else {
    while (s21_isspace(str[*str_p])) (*str_p)++;

    s21_size_t s_len = 0;
    wchar_t s[256] = L"\0";
    while ((!s21_isspace(str[*str_p]) && str[*str_p] != '\0') &&
           (tok.width == -1 || s_len < (s21_size_t)tok.width)) {
      s_len++;
      s[s_len - 1] = (wchar_t)str[*str_p];
      (*str_p)++;
    }

    if (tok.star == -1) {
      write_s(arg, tok, s, s_len, var_cnt);
    }
  }
}

void write_s(va_list arg, token tok, wchar_t *s, s21_size_t s_len,
             int *var_cnt) {
  void *var = va_arg(arg, void *);
  if (tok.length == 'l') {
    for (s21_size_t i = 0; i < s_len; i++)
      *((wchar_t *)var + i) = (wchar_t)s[i];
    *((wchar_t *)var + s_len) = (wchar_t)'\0';
  } else if (tok.length == -1) {
    for (s21_size_t i = 0; i < s_len; i++) *((char *)var + i) = (char)s[i];
    *((char *)var + s_len) = (char)'\0';
  }
  (*var_cnt)++;
}

void init_dec(s21_size_t *str_p, const char *str, va_list arg, token tok,
              int *var_cnt, int *err, int *run, int sign) {
  if ((tok.width <= 0 && tok.width != -1) ||
      (tok.length != -1 && tok.length != 'h' && tok.length != 'l'))
    *err = 1;
  else if (is_spaced_string(str) || str[0] == '\0')
    *var_cnt = -1;
  else {
    while (s21_isspace(str[*str_p])) (*str_p)++;

    s21_size_t s_len = 0;
    char s[50] = "\0";
    if (str[*str_p] == '-' || str[*str_p] == '+') {
      s[0] = str[*str_p];
      s_len++;
      (*str_p)++;
    }

    while (((is_digit(str[*str_p]) && str[*str_p] != '\0') ||
            str[*str_p] == 'i') &&
           (tok.width == -1 || s_len < (s21_size_t)tok.width)) {
      s_len++;
      s[s_len - 1] = str[*str_p];
      (*str_p)++;
    }
    s[s_len] = '\0';

    if (tok.star == -1 && s_len > 0) {
      s21_size_t i = 0;
      if (s[i] == '-' || s[i] == '+') i++;
      if (s[i] != '\0')
        write_dec(arg, tok, s, s_len, var_cnt, sign);
      else
        *run = 0;
    }
  }
}

void init_e_num(s21_size_t *str_p, const char *str, va_list arg, token tok,
                int *var_cnt, char *s, s21_size_t *s_len, int *sign_flag) {
  while ((tok.width == -1 || *s_len < (s21_size_t)tok.width) && str[*str_p] &&
         is_digit(str[*str_p])) {
    s[(*s_len)++] = str[*str_p];
    (*str_p)++;
  }
  if ((tok.width == -1 || *s_len < (s21_size_t)tok.width) &&
      str[*str_p] == '.') {
    if (!(*s_len) && str[(*str_p) + 1] && is_digit(str[(*str_p) + 1])) {
      s[(*s_len)++] = '0';
      if (tok.width != -1) tok.width++;
    }
    do {
      s[(*s_len)++] = str[*str_p];
      (*str_p)++;
    } while ((tok.width == -1 || *s_len < (s21_size_t)tok.width) &&
             str[*str_p] && is_digit(str[*str_p]));
  }
  if ((tok.width == -1 || *s_len < (s21_size_t)tok.width) &&
      str[*str_p] == 'e') {
    s[(*s_len)++] = str[*str_p];
    (*str_p)++;
    if (str[*str_p] == '-' || str[*str_p] == '+') {
      s[(*s_len)++] = str[*str_p];
      (*str_p)++;
    }
    while ((tok.width == -1 || (*s_len) < (s21_size_t)tok.width) &&
           str[*str_p] && is_digit(str[*str_p])) {
      s[(*s_len)++] = str[*str_p];
      (*str_p)++;
    }
  }
  if (tok.star == -1 && *s_len > (s21_size_t)(*sign_flag) &&
      !(s[0] == '.' || s[0] == 'e' ||
        ((s[0] == '+' || s[0] == '-') && (s[1] == '.' || s[1] == 'e'))))
    write_float(arg, tok, s, *s_len, var_cnt);
}

void init_e(s21_size_t *str_p, const char *str, va_list arg, token tok,
            int *var_cnt, int *err) {
  int sign_flag = 0;
  if ((tok.width <= 0 && tok.width != -1) ||
      (tok.length != 'L' && tok.length != -1 && tok.length != 'l'))
    *err = 1;
  else if (is_spaced_string(str) || str[0] == '\0')
    *var_cnt = -1;
  else {
    while (str[*str_p] && s21_isspace(str[*str_p])) (*str_p)++;
    s21_size_t s_len = 0;
    char s[500] = "\0";
    if (str[*str_p] == '-' || str[*str_p] == '+') {
      s[s_len++] = str[*str_p];
      (*str_p)++;
      sign_flag = 1;
    }
    if ((3 <= tok.width || tok.width == -1) && str[*str_p] != '\0' &&
        str[*str_p + 1] != '\0' && str[*str_p + 2] != '\0' &&
        s21_strchr("iI", str[*str_p]) != s21_NULL &&
        s21_strchr("nN", str[*str_p + 1]) != s21_NULL &&
        s21_strchr("fF", str[*str_p + 2]) != s21_NULL) {
      (*str_p) += 3;
      if (tok.star == -1) {
        if (s[0] == '-')
          write_inf(arg, tok, var_cnt, -1);
        else
          write_inf(arg, tok, var_cnt, 1);
      }
    } else if ((3 <= tok.width || tok.width == -1) && str[*str_p] != '\0' &&
               str[*str_p + 1] != '\0' && str[*str_p + 2] != '\0' &&
               s21_strchr("nN", str[*str_p]) != s21_NULL &&
               s21_strchr("aA", str[*str_p + 1]) != s21_NULL &&
               s21_strchr("nN", str[*str_p + 2]) != s21_NULL) {
      (*str_p) += 3;
      if (tok.star == -1) {
        write_nan(arg, tok, var_cnt);
      }
    } else {
      init_e_num(str_p, str, arg, tok, var_cnt, s, &s_len, &sign_flag);
    }
  }
}

void write_inf(va_list arg, token tok, int *var_cnt, int sign) {
  void *var = va_arg(arg, void *);
  if (tok.length == 'L') {
    *((long double *)var) = (long double)INFINITY * sign;
  } else if (tok.length == -1) {
    *((float *)var) = (float)INFINITY * sign;
  } else if (tok.length == 'l') {
    *((double *)var) = (double)INFINITY * sign;
  }
  (*var_cnt)++;
}

void write_nan(va_list arg, token tok, int *var_cnt) {
  void *var = va_arg(arg, void *);
  if (tok.length == 'L') {
    *((long double *)var) = (long double)nan("");
  } else if (tok.length == -1) {
    *((float *)var) = (float)nan("");
  } else if (tok.length == 'l') {
    *((double *)var) = (double)nan("");
  }
  (*var_cnt)++;
}

void write_float(va_list arg, token tok, char *s, s21_size_t s_len,
                 int *var_cnt) {
  void *var = va_arg(arg, void *);
  if (tok.length == 'L') {
    *((long double *)var) = s21_atold(s, s_len);
  } else if (tok.length == -1) {
    *((float *)var) = (float)s21_atold(s, s_len);
  } else if (tok.length == 'l') {
    *((double *)var) = (double)s21_atold(s, s_len);
  }
  (*var_cnt)++;
}

void init_i(s21_size_t *str_p, const char *str, va_list arg, token tok,
            int *var_cnt, int *err) {
  if ((tok.width <= 0 && tok.width != -1) ||
      (tok.length != -1 && tok.length != 'h' && tok.length != 'l'))
    *err = 1;
  else if (is_spaced_string(str) || str[0] == '\0')
    *var_cnt = -1;
  else {
    int sign_flag = 0;
    while (s21_isspace(str[*str_p])) (*str_p)++;
    s21_size_t s_len = 0;
    char s[50] = "\0";
    if (str[*str_p] == '-' || str[*str_p] == '+') {
      s[0] = str[*str_p];
      s_len++;
      (*str_p)++;
      sign_flag = 1;
    }
    int ss = 10;
    if (str[*str_p] == '0' && tok.width != 1) {
      (*str_p)++;
      if (str[*str_p] && is_digit(str[*str_p])) {
        if (tok.width != -1) tok.width--;
        ss = 8;
      } else if (str[*str_p] && (str[*str_p] == 'x' || str[*str_p] == 'X')) {
        (*str_p)++;
        if (tok.width != -1) tok.width -= 2;
        ss = 16;
      } else {
        s[s_len++] = '0';
      }
    }
    while (str[*str_p] &&
           ((ss == 10 && is_digit(str[*str_p])) ||
            (ss == 8 && is_oct(str[*str_p])) ||
            (ss == 16 && is_hex(str[*str_p]))) &&
           (tok.width == -1 || s_len < (s21_size_t)tok.width)) {
      s_len++;
      s[s_len - 1] = str[*str_p];
      (*str_p)++;
    }
    s[s_len] = '\0';
    if (tok.star == -1 && s_len > 0) {
      if (ss == 10 && ((s_len > 0 && !sign_flag) || (s_len > 1 && sign_flag)))
        write_dec(arg, tok, s, s_len, var_cnt, 1);
      else if (ss == 8)
        write_oct(arg, tok, s, s_len, var_cnt, 1);
      else if (ss == 16)
        write_hex(arg, tok, s, s_len, var_cnt, 1);
    }
  }
}

void init_o(s21_size_t *str_p, const char *str, va_list arg, token tok,
            int *var_cnt, int *err) {
  int prefix_flag = 0;
  int sign_flag = 0;
  if ((tok.width <= 0 && tok.width != -1) ||
      (tok.length != -1 && tok.length != 'h' && tok.length != 'l'))
    *err = 1;
  else if (is_spaced_string(str) || str[0] == '\0')
    *var_cnt = -1;
  else {
    while (s21_isspace(str[*str_p])) (*str_p)++;

    s21_size_t s_len = 0;
    char s[50] = "\0";
    if (str[*str_p] == '-' || str[*str_p] == '+') {
      s[s_len++] = str[(*str_p)++];
      sign_flag = 1;
    }
    if (str[*str_p] == '0' &&
        (tok.width == -1 || s_len < (s21_size_t)tok.width)) {
      prefix_flag = 1;
    }
    while (str[*str_p] && is_oct(str[*str_p]) &&
           (tok.width == -1 || s_len < (s21_size_t)tok.width)) {
      s_len++;
      s[s_len - 1] = str[*str_p];
      (*str_p)++;
    }
    s[s_len] = '\0';
    if (tok.star == -1 && (s_len > (s21_size_t)sign_flag) &&
        (tok.width > prefix_flag || tok.width == -1)) {
      write_oct(arg, tok, s, s_len, var_cnt, 0);
    }
  }
}

void init_x(s21_size_t *str_p, const char *str, va_list arg, token tok,
            int *var_cnt, int *err) {
  int prefix_flag = 0;
  int sign_flag = 0;
  if ((tok.width <= 0 && tok.width != -1) ||
      (tok.length != -1 && tok.length != 'h' && tok.length != 'l'))
    *err = 1;
  else if (is_spaced_string(str) || str[0] == '\0')
    *var_cnt = -1;
  else {
    while (s21_isspace(str[*str_p])) (*str_p)++;
    s21_size_t s_len = 0;
    char s[50] = "\0";
    if (str[*str_p] == '-' || str[*str_p] == '+') {
      s[0] = str[*str_p];
      s_len++;
      (*str_p)++;
      sign_flag = 1;
    }

    if (str[*str_p] == '0' &&
        (str[*str_p + 1] == 'x' || str[*str_p + 1] == 'X')) {
      tok.width -= 2;
      (*str_p) += 2;
      prefix_flag = 1;
    }

    while ((tok.width == -1 || s_len < (s21_size_t)tok.width) && str[*str_p] &&
           is_hex(str[*str_p])) {
      s_len++;
      s[s_len - 1] = str[*str_p];
      (*str_p)++;
    }
    s[s_len] = '\0';

    if (tok.star == -1 && (s_len > (s21_size_t)sign_flag || prefix_flag)) {
      write_hex(arg, tok, s, s_len, var_cnt, 0);
    }
  }
}

void write_dec(va_list arg, token tok, char *s, s21_size_t s_len, int *var_cnt,
               int sign) {
  void *var = va_arg(arg, void *);
  long long res = s21_atoi(s, s_len);
  if (sign) {
    if (tok.length == 'h')
      *((short int *)var) = (short)res;
    else if (tok.length == -1)
      *((int *)var) = (int)res;
    else if (tok.length == 'l')
      *((long int *)var) = res;
  } else {
    if (tok.length == 'h')
      *((unsigned short int *)var) = (unsigned short int)res;
    else if (tok.length == -1)
      *((unsigned int *)var) = (unsigned int)res;
    else if (tok.length == 'l')
      *((unsigned long int *)var) = (unsigned long int)res;
  }
  (*var_cnt)++;
}

void write_oct(va_list arg, token tok, char *s, s21_size_t s_len, int *var_cnt,
               int sign) {
  void *var = va_arg(arg, void *);
  long long res = oct_to_dec(s, s_len);
  if (sign) {
    if (tok.length == 'h')
      *((short int *)var) = (short int)res;
    else if (tok.length == -1)
      *((int *)var) = (int)res;
    else if (tok.length == 'l')
      *((long int *)var) = (long int)res;
  } else {
    if (tok.length == 'h')
      *((unsigned short int *)var) = (unsigned short int)res;
    else if (tok.length == -1)
      *((unsigned int *)var) = (unsigned int)res;
    else if (tok.length == 'l')
      *((unsigned long int *)var) = (unsigned long int)res;
  }
  (*var_cnt)++;
}

void write_hex(va_list arg, token tok, char *s, s21_size_t s_len, int *var_cnt,
               int sign) {
  void *var = va_arg(arg, void *);
  long long res = hex_to_dec(s, s_len);
  if (sign) {
    if (tok.length == 'h')
      *((short int *)var) = (short int)res;
    else if (tok.length == -1)
      *((int *)var) = (int)res;
    else if (tok.length == 'l')
      *((long long *)var) = (long long)res;
  } else {
    if (tok.length == 'h')
      *((unsigned short int *)var) = (unsigned short int)res;
    else if (tok.length == -1)
      *((unsigned int *)var) = (unsigned int)res;
    else if (tok.length == 'l')
      *((unsigned long long *)var) = (unsigned long long)res;
  }
  (*var_cnt)++;
}

void init_p(s21_size_t *str_p, const char *str, va_list arg, token tok,
            int *var_cnt, int *err) {
  if ((tok.width <= 0 && tok.width != -1) || tok.length != -1)
    *err = 1;
  else if (is_spaced_string(str) || str[0] == '\0')
    *var_cnt = -1;
  else {
    while (s21_isspace(str[*str_p])) (*str_p)++;

    s21_size_t s_len = 0;
    char s[50] = "\0";

    if (str[*str_p] == '0' &&
        (str[*str_p + 1] == 'x' || str[*str_p + 1] == 'X')) {
      if (tok.width != -1) tok.width -= 2;
      (*str_p) += 2;
    }
    while (str[*str_p] && is_hex(str[*str_p]) &&
           (tok.width == -1 || s_len < (s21_size_t)tok.width)) {
      s[s_len++] = str[(*str_p)++];
    }
    s[s_len] = '\0';

    if (tok.star == -1 && s_len > 0) {
      write_ptr(arg, s, s_len);
    }
    (*var_cnt)++;
  }
}

void write_ptr(va_list arg, char *s, s21_size_t s_len) {
  void **var = va_arg(arg, void **);
  long long res = hex_to_dec(s, s_len);
  *var = (void *)(res);
}

void init_n(int str_p, va_list arg, token tok, int *err) {
  if (tok.width != -1 ||
      (tok.length != 'l' && tok.length != 'h' && tok.length != -1))
    *err = 1;
  else {
    if (tok.star == -1) {
      write_n(arg, tok, str_p);
    }
  }
}

void write_n(va_list arg, token tok, int str_p) {
  void *var = va_arg(arg, void *);
  if (tok.length == 'h')
    *((short int *)var) = (short int)str_p;
  else if (tok.length == 'l')
    *((long int *)var) = (long int)str_p;
  else if (tok.length == -1)
    *((int *)var) = (int)str_p;
}

void init_percent(s21_size_t *str_p, const char *str, token tok, int *err,
                  int *run) {
  if (tok.star != -1 || tok.width != -1 || tok.length != -1) {
    *err = 1;
  } else {
    while (s21_isspace(str[*str_p])) {
      (*str_p)++;
    }
    if (str[*str_p] == '%')
      (*str_p)++;
    else
      *run = 1;
  }
}

int is_spaced_string(const char *str) {
  int res = 1;
  for (s21_size_t i = 0; str[i] != '\0' && res; i++) {
    if (!s21_isspace(str[i])) res = 0;
  }
  return res;
}