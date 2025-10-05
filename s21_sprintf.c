#include "s21_sprintf.h"

int s21_sprintf(char *buffer, const char *format_string, ...) {
  va_list args;
  va_start(args, format_string);
  int out_index = 0;
  int err = 0;
  while (*format_string != '\0' && (!err)) {
    if (*format_string != '%') {
      buffer[out_index++] = *format_string;
      format_string++;
      continue;
    }
    const char *begin_format = format_string;
    tFormat format = {0};
    format_string += parse_format(&format, format_string);
    validate_format(&format);

    if (format.flags & F_WIDTH_ARG) format.width = va_arg(args, int);
    if (format.flags & F_PRECISION_ARG) format.precision = va_arg(args, int);

    if (format.type_spec == 'n') {
      int *n = va_arg(args, int *);
      *n = out_index;
    }

    int result = handle_chars(buffer + out_index, format, &args);
    if (result < 0)
      err = 1;
    else
      out_index += result;
    out_index += handle_integers(buffer + out_index, format, &args);
    out_index += handle_reals(buffer + out_index, format, &args);

    if (format.type_spec == '%') buffer[out_index++] = '%';
    if (format.type_spec == '\0') {
      s21_strncpy(buffer + out_index, begin_format,
                  format_string - begin_format);
      out_index += format_string - begin_format;
    }
  }
  buffer[out_index] = '\0';
  va_end(args);
  return err ? -1 : out_index;
}

s21_size_t parse_format(tFormat *p_format, const char *format_string) {
  s21_size_t offset = 1;
  offset += parse_flags(p_format, format_string + offset);
  offset += parse_width(p_format, format_string + offset);
  offset += parse_precision(p_format, format_string + offset);
  offset += parse_size(p_format, format_string + offset);
  offset += parse_type_spec(p_format, format_string + offset);
  return offset;
}

s21_size_t parse_flags(tFormat *p_format, const char *format_string) {
  s21_size_t offset = 0;
  int flag_index = get_flag_index(format_string[offset]);
  while (flag_index != -1) {
    p_format->flags |= (1 << flag_index);
    offset++;
    flag_index = get_flag_index(format_string[offset]);
  }
  return offset;
}

int get_flag_index(const char ch) {
  int index = 0;
  char *flags = CFLAGS;
  while (ch != flags[index] && flags[index]) index++;
  return flags[index] ? index : -1;
}

s21_size_t parse_width(tFormat *p_format, const char *format_string) {
  s21_size_t offset = 0;
  if (format_string[offset] == '*') {
    p_format->flags |= (F_WIDTH_SET | F_WIDTH_ARG);
    offset++;
  } else {
    unsigned width = 0;
    offset += parse_number(&width, format_string);
    if (width) {
      p_format->flags |= F_WIDTH_SET;
      p_format->width = width;
    }
  }
  return offset;
}

s21_size_t parse_precision(tFormat *p_format, const char *format_string) {
  s21_size_t offset = 0;
  if (format_string[offset] == '.') {
    p_format->flags |= F_PRECISION_SET;
    offset++;
    if (format_string[offset] == '*') {
      p_format->flags |= F_PRECISION_ARG;
      offset++;
    } else {
      offset += parse_number(&(p_format->precision), format_string + offset);
    }
  }
  return offset;
}

s21_size_t parse_number(unsigned *number, const char *format_string) {
  *number = 0;
  s21_size_t offset = 0;
  while (format_string[offset] > 47 && format_string[offset] < 58) {
    *number = (*number * 10) + (format_string[offset] - 48);
    offset++;
  }
  return offset;
}

s21_size_t parse_size(tFormat *p_format, const char *format_string) {
  s21_size_t offset = 0;
  unsigned index = 0;
  char *size_modifiers = CSIZE;
  while (format_string[offset] != size_modifiers[index] &&
         size_modifiers[index])
    index++;
  if (size_modifiers[index]) offset++;
  p_format->size = size_modifiers[index];
  return offset;
}

s21_size_t parse_type_spec(tFormat *p_format, const char *format_string) {
  s21_size_t offset = 0;
  int index = 0;
  char *type_specifiers = CSPECS;
  while (format_string[offset] != type_specifiers[index] &&
         type_specifiers[index])
    index++;
  if (type_specifiers[index]) offset++;
  p_format->type_spec = type_specifiers[index];
  return offset;
}

void validate_format(tFormat *format) {
  const tFormat right_struct[] = AVAILABLE_FLAGS;
  for (int i = 0; i < 16; i++) {
    if (right_struct[i].type_spec == format->type_spec) {
      format->flags = format->flags & right_struct[i].flags;
    }
  }
}

s21_size_t handle_chars(char *buffer, tFormat format, va_list *args) {
  s21_size_t out_index = 0;
  if (format.type_spec == 'c') {
    if (format.size == 'l') {
      wchar_t wch = va_arg(*args, wchar_t);
      out_index += out_string(buffer + out_index, format, (char *)&wch);
    } else {
      char ch = va_arg(*args, int);
      out_index += out_string(buffer + out_index, format, &ch);
    }
  }
  if (format.type_spec == 's')
    out_index += out_string(buffer + out_index, format, va_arg(*args, char *));
  return out_index;
}

s21_size_t handle_integers(char *buffer, tFormat format, va_list *args) {
  s21_size_t out_index = 0;
  if (format.type_spec == 'd' || format.type_spec == 'i') {
    long int signed_int =
        format.size == 'l' ? va_arg(*args, long int)
                           : (format.size == 'h' ? (short int)va_arg(*args, int)
                                                 : va_arg(*args, int));
    if (signed_int < 0) {
      signed_int = -signed_int;
      format.type_spec = '-';
    }
    out_index += out_integer(buffer + out_index, format, signed_int);
  }
  if (format.type_spec == 'u' || format.type_spec == 'o' ||
      format.type_spec == 'x' || format.type_spec == 'X') {
    unsigned long unsigned_int =
        (format.size == 'h')
            ? (unsigned short)va_arg(*args, unsigned)
            : ((format.size == 'l') ? va_arg(*args, unsigned long)
                                    : va_arg(*args, unsigned));
    out_index += out_integer(buffer + out_index, format, unsigned_int);
  }
  if (format.type_spec == 'p') {
    unsigned long pointer = va_arg(*args, unsigned long);
#if defined __linux__
    if (pointer) {
#endif
      format.type_spec = 'x';
      format.flags = format.flags | F_SPECIAL_VIEW;
      out_index += out_integer(buffer + out_index, format, pointer);
#if defined __linux__
    } else {
      format.flags &= ~F_PRECISION_SET;
      format.size = 0;
      out_index += out_string(buffer + out_index, format, S_NULL_POINTER);
    }
#endif
  }
  return out_index;
}

s21_size_t handle_reals(char *buffer, tFormat format, va_list *args) {
  s21_size_t out_index = 0;
  if (format.type_spec == 'f' || format.type_spec == 'e' ||
      format.type_spec == 'E' || format.type_spec == 'g' ||
      format.type_spec == 'G') {
    long double signed_double =
        format.size == 'L' ? va_arg(*args, long double) : va_arg(*args, double);
    out_index += out_double(buffer + out_index, format, signed_double);
  }
  return out_index;
}

int out_string(char *buffer, const tFormat format, const char *str) {
  char str_buff[MAXBUFSIZE] = {0};
  int length = 0;
  if (!str) {
    length = s21_strlen(S_NULL_STRING);
    if (!(format.flags & F_PRECISION_SET))
      s21_strncpy(str_buff, S_NULL_STRING, length);
    else if (length <= (int)format.precision)
      s21_strncpy(str_buff, S_NULL_STRING, length);
    else
      length = 0;
  } else {
    if (format.size == 'l') {
      if (format.type_spec == 'c') length = wctomb(str_buff, *((wchar_t *)str));
      if (format.type_spec == 's')
        length = wcstombs(str_buff, (wchar_t *)str, MAXBUFSIZE);

    } else {
      length = format.type_spec == 'c' ? 1 : s21_strlen(str);
      s21_strncpy(str_buff, str, length);
    }
  }
  if ((format.flags & F_PRECISION_SET) && ((int)format.precision < length))
    length = format.precision;
  return (length >= 0)
             ? justify_formatted_string(buffer, format, str_buff, length)
             : length;
}

s21_size_t out_integer(char *buffer, const tFormat format,
                       const unsigned long number) {
  int base = 10;
  if (format.type_spec == 'x' || format.type_spec == 'X')
    base = 16;
  else if (format.type_spec == 'o')
    base = 8;
  char numbuff[MAXBUFSIZE] = {0};
  s21_size_t num_length = s21_itoa(numbuff + MAXBUFSIZE - 2, number, base);
  if ((format.flags & F_PRECISION_SET) && !format.precision && !number)
    num_length = 1;
  s21_size_t offset = MAXBUFSIZE - num_length;

  if (format.flags & F_PRECISION_SET)
    offset += format_with_precision(numbuff + offset, format);
  else if (!(format.flags & F_ADJAST_LEFT) && (format.flags & F_LEADING_ZERO)) {
    s21_size_t len_prefix = get_prefix_length(format, number);
    if (len_prefix + num_length - 1 < format.width) {
      s21_size_t z_length = format.width - (len_prefix + num_length - 1);
      offset -= z_length;
      s21_memset(numbuff + offset, '0', z_length);
    }
  }

  offset += set_prefix(numbuff + offset, format, number);

  s21_size_t total_length = MAXBUFSIZE - 1 - offset;
  if (format.type_spec == 'X') {
    char *upper_str = s21_to_upper(numbuff + offset);
    s21_strncpy(numbuff + offset, upper_str, total_length);
    free(upper_str);
  }
  total_length =
      justify_formatted_string(buffer, format, numbuff + offset, total_length);
  buffer[total_length] = 0;
  return total_length;
}

s21_size_t get_prefix_length(tFormat format, const unsigned long number) {
  s21_size_t length = 0;
  if (format.type_spec == '-')
    length++;
  else if (format.flags & F_FORCE_SIGN ||
           format.flags & F_SPACE_BEFORE_POSITIVE)
    length++;
  if (number && format.flags & F_SPECIAL_VIEW &&
      (format.type_spec == 'x' || format.type_spec == 'X'))
    length += 2;
  return length;
}

int format_with_precision(char *numbuff, tFormat format) {
  int offset = 0;
  s21_size_t num_length = s21_strlen(numbuff);

  if (format.precision > num_length) {
    offset = format.precision - num_length;
    s21_memset(numbuff - offset, '0', offset);
    offset = -offset;
  }

  return offset;
}

int set_prefix(char *numbuff, tFormat format, const unsigned long number) {
  int offset = 0;

  if (format.type_spec == 'o' && (format.flags & F_SPECIAL_VIEW) &&
      (numbuff[offset] != '0'))
    numbuff[--offset] = '0';
  if ((number) && (format.type_spec == 'x' || format.type_spec == 'X') &&
      format.flags & F_SPECIAL_VIEW) {
    numbuff[--offset] = 'x';
    numbuff[--offset] = '0';
  }
  if (format.type_spec == '-')
    numbuff[--offset] = '-';
  else if (format.flags & F_FORCE_SIGN)
    numbuff[--offset] = '+';
  else if (format.flags & F_SPACE_BEFORE_POSITIVE)
    numbuff[--offset] = ' ';
  return offset;
}

int justify_formatted_string(char *buffer, const tFormat format,
                             const char *str, const s21_size_t length) {
  int spaces = 0;
  if ((format.flags & F_WIDTH_SET) && (format.width > length)) {
    spaces = format.width - length;
    if (!(format.flags & F_ADJAST_LEFT)) {
      s21_memset(buffer, ' ', spaces);
      s21_strncpy(buffer + spaces, str, length);
    } else {
      s21_strncpy(buffer, str, length);
      s21_memset(buffer + length, ' ', spaces);
    }
  } else
    s21_strncpy(buffer, str, length);
  buffer[length + spaces] = '\0';
  return length + spaces;
}

s21_size_t s21_itoa(char *buffer, const unsigned long number, const int base) {
  s21_size_t index = 0;
  const char *cyphers = "0123456789abcdef";
  unsigned long tens = number;
  do {
    *(buffer - index) = cyphers[tens % base];
    index++;
    tens = tens / base;
  } while (tens > 0);
  return index + 1;
}

s21_size_t inf_or_nan(char *buffer, const long double number) {
  s21_size_t index = 0;
  if (isnan(number)) {
    buffer[-index++] = 'n';
    buffer[-index++] = 'a';
    buffer[-index++] = 'n';
  } else {
    buffer[-index++] = 'f';
    buffer[-index++] = 'n';
    buffer[-index++] = 'i';
  }
  return index + 1;
}

s21_size_t s21_ftoa(char *buffer, const long double number, tFormat format,
                    unsigned flag_g) {
  s21_size_t index = 0;
  long long unsigned num_int =
      (long long unsigned)(roundl(number * pow(10, format.precision)) /
                           pow(10, format.precision));
  long long unsigned num_after_dot_int =
      (long long unsigned)(roundl((number - (long double)num_int) *
                                  pow(10, format.precision)) +
                           pow(10, format.precision));
  while (flag_g && !(num_after_dot_int % 10) &&
         !(format.flags & F_SPECIAL_VIEW)) {
    num_after_dot_int = num_after_dot_int / 10;
    format.precision -= 1;
  }
  if (format.precision) {
    index += s21_itoa(buffer - index, num_after_dot_int, 10) - 1;
    buffer[-(index - 1)] = '.';
  } else if (format.flags & F_SPECIAL_VIEW) {
    buffer[-(index++)] = '.';
  }
  index += s21_itoa(buffer - index, num_int, 10) - 1;
  return index + 1;
}

void find_exp(int *exp, long double *number, int precision) {
  while ((roundl(*number * pow(10, precision + 1)) / pow(10, precision + 1)) >=
         10) {
    *exp = *exp + 1;
    *number /= 10;
  }
  while ((roundl(*number * pow(10, precision + 1)) / pow(10, precision + 1)) <
             1 &&
         *number != 0) {
    *exp = *exp - 1;
    *number *= 10;
  }
}

s21_size_t s21_ntoa(char *buffer, const long double number, tFormat format,
                    unsigned flag_g) {
  s21_size_t index = 0;
  int exp = 0, neg_flag_exp = 0;
  long double number_buf = (long double)number;
  find_exp(&exp, &number_buf, format.precision);
  if (exp < 0) {
    neg_flag_exp = 1;
    exp *= -1;
  }

  long long int num_int =
      (long long int)(roundl(number_buf * pow(10, format.precision)) /
                      pow(10, format.precision));
  long long int num_after_dot_int =
      roundl((number_buf - num_int + 1) * pow(10, format.precision));
  index += s21_itoa(buffer - index, exp, 10) - 1;
  if (index < 2) {
    buffer[-index++] = '0';
  }
  if (neg_flag_exp) {
    buffer[-index++] = '-';
  } else {
    buffer[-index++] = '+';
  }
  buffer[-index++] = 'e';
  while (flag_g && !(num_after_dot_int % 10) &&
         !(format.flags & F_SPECIAL_VIEW)) {
    num_after_dot_int = num_after_dot_int / 10;
    format.precision -= 1;
  }
  if (format.precision) {
    index += s21_itoa(buffer - index, num_after_dot_int, 10) - 1;
    buffer[-(index - 1)] = '.';
  } else if (format.flags & F_SPECIAL_VIEW) {
    buffer[-(index++)] = '.';
  }
  index += s21_itoa(buffer - index, num_int, 10) - 1;
  return index + 1;
}

s21_size_t out_double(char *buffer, tFormat format, const long double number) {
  int type_spec_store = format.type_spec;
  s21_size_t num_length = 0;
  long double num_unsign = (long double)number;
  if (number < 0) {
    num_unsign *= -1;
    format.type_spec = '-';
  }
  char numbuff[MAXBUFSIZE] = {0};
  if (!(format.flags & F_PRECISION_ARG || format.flags & F_PRECISION_SET)) {
    format.precision = 6;
  }
  if (!isnan(num_unsign) && !isinf(num_unsign)) {
    if (type_spec_store == 'f') {
      num_length = s21_ftoa(numbuff + MAXBUFSIZE - 2, num_unsign, format, 0);
    } else if (type_spec_store == 'e' || type_spec_store == 'E') {
      num_length = s21_ntoa(numbuff + MAXBUFSIZE - 2, num_unsign, format, 0);
    } else {
      num_length = s21_n_or_f_toa(numbuff + MAXBUFSIZE - 2, num_unsign, format,
                                  &type_spec_store);
    }
  } else {
    num_length = inf_or_nan(numbuff + MAXBUFSIZE - 2, num_unsign);
  }

  s21_size_t offset = MAXBUFSIZE - num_length;
  if (!(format.flags & F_ADJAST_LEFT) && (format.flags & F_LEADING_ZERO)) {
    s21_size_t len_prefix = get_prefix_length(format, num_unsign);
    if (len_prefix + num_length - 1 < format.width) {
      s21_size_t z_length = format.width - (len_prefix + num_length - 1);
      offset -= z_length;
      s21_memset(numbuff + offset, '0', z_length);
    }
  }
  offset += set_prefix(numbuff + offset, format, num_unsign);
  s21_size_t total_length = MAXBUFSIZE - 1 - offset;
  if (type_spec_store == 'E') {
    char *upper_str = s21_to_upper(numbuff + offset);
    s21_strncpy(numbuff + offset, upper_str, total_length);
    free(upper_str);
  }
  total_length =
      justify_formatted_string(buffer, format, numbuff + offset, total_length);
  buffer[total_length] = 0;
  return total_length;
}

int compare_len(long double number) {
  int res = 0;
  while (number >= 1) {
    res++;
    number /= 10;
  }
  return res;
}

s21_size_t s21_n_or_f_toa(char *numbuff, long double number, tFormat format,
                          int *type_spec_store) {
  s21_size_t index = 10;
  int exp = 0;
  long double number_buf = (long double)number;
  int accuracy = 0;
  if (format.precision != 0) {
    accuracy = format.precision - 1;
  }
  find_exp(&exp, &number_buf, accuracy);

  if (exp <= accuracy && exp >= -4) {
    format.precision = accuracy - (exp);
    index = s21_ftoa(numbuff, number, format, 1);
  } else if (*type_spec_store == 'g') {
    format.precision = accuracy;
#if defined(__linux__)
    if (exp == accuracy + 1 && format.flags & F_SPECIAL_VIEW &&
        compare_len(roundl(number)) > compare_len(number)) {
      format.precision = 0;
    }
#endif
    index = s21_ntoa(numbuff, number, format, 1);
    *type_spec_store = 'e';
  } else {
    format.precision = accuracy;
#if defined(__linux__)
    if (exp == accuracy + 1 && format.flags & F_SPECIAL_VIEW &&
        compare_len(roundl(number)) > compare_len(number)) {
      format.precision = 0;
    }
#endif
    index = s21_ntoa(numbuff, number, format, 1);
    *type_spec_store = 'E';
  }
  return index;
}
