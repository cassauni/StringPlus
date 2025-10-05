#ifndef S21_SPRINTF_H
#define S21_SPRINTF_H

#include <math.h>
#include <stdarg.h>
#include <wchar.h>

#include "s21_string.h"

#define MAXBUFSIZE 4096

#define S_NULL_POINTER "(nil)"
#define S_NULL_STRING "(null)"

#define CFLAGS "-+ #0"
#define CSIZE "hlL"
#define CSPECS "cdieEfgGosuxXpn%"

#define F_ADJAST_LEFT 1
#define F_FORCE_SIGN 2
#define F_SPACE_BEFORE_POSITIVE 4
#define F_SPECIAL_VIEW 8
#define F_LEADING_ZERO 16
#define F_WIDTH_SET 32
#define F_WIDTH_ARG 64
#define F_PRECISION_SET 128
#define F_PRECISION_ARG 256

#define BIN9(b8, b7, b6, b5, b4, b3, b2, b1, b0)                         \
  (b8 * 256 + b7 * 128 + b6 * 64 + b5 * 32 + b4 * 16 + b3 * 8 + b2 * 4 + \
   b1 * 2 + b0)
#define AVAILABLE_FLAGS                                                       \
  {                                                                           \
    {'c', BIN9(0, 0, 1, 1, 0, 0, 0, 0, 1), 0, 0, 0},                          \
        {'s', BIN9(1, 1, 1, 1, 0, 0, 0, 0, 1), 0, 0, 0},                      \
        {'d', BIN9(1, 1, 1, 1, 1, 0, 1, 1, 1), 0, 0, 0},                      \
        {'i', BIN9(1, 1, 1, 1, 1, 0, 1, 1, 1), 0, 0, 0},                      \
        {'e', BIN9(1, 1, 1, 1, 1, 1, 1, 1, 1), 0, 0, 0},                      \
        {'E', BIN9(1, 1, 1, 1, 1, 1, 1, 1, 1), 0, 0, 0},                      \
        {'f', BIN9(1, 1, 1, 1, 1, 1, 1, 1, 1), 0, 0, 0},                      \
        {'g', BIN9(1, 1, 1, 1, 1, 1, 1, 1, 1), 0, 0, 0},                      \
        {'G', BIN9(1, 1, 1, 1, 1, 1, 1, 1, 1), 0, 0, 0},                      \
        {'o', BIN9(1, 1, 1, 1, 1, 1, 0, 0, 1), 0, 0, 0},                      \
        {'u', BIN9(1, 1, 1, 1, 1, 0, 0, 0, 1), 0, 0, 0},                      \
        {'x', BIN9(1, 1, 1, 1, 1, 1, 0, 0, 1), 0, 0, 0},                      \
        {'X', BIN9(1, 1, 1, 1, 1, 1, 0, 0, 1), 0, 0, 0},                      \
        {'p', BIN9(1, 1, 1, 1, 1, 0, 1, 1, 1), 0, 0, 0}, {'n', 0, 0, 0, 0}, { \
      '%', 0, 0, 0, 0                                                         \
    }                                                                         \
  }

typedef struct {
  char type_spec;
  unsigned flags;
  unsigned width;
  unsigned precision;
  char size;
} tFormat;

int s21_sprintf(char* buffer, const char* format_string, ...);

s21_size_t parse_format(tFormat* p_format, const char* format_string);
s21_size_t parse_flags(tFormat* p_format, const char* format_string);
int get_flag_index(const char ch);
s21_size_t parse_width(tFormat* p_format, const char* format_string);
s21_size_t parse_precision(tFormat* p_format, const char* format_string);
s21_size_t parse_number(unsigned* number, const char* format_string);
s21_size_t parse_size(tFormat* p_format, const char* format_string);
s21_size_t parse_type_spec(tFormat* p_format, const char* format_string);
void validate_format(tFormat* format);

s21_size_t handle_chars(char* buffer, tFormat format, va_list* args);
s21_size_t handle_integers(char* buffer, tFormat format, va_list* args);
s21_size_t handle_reals(char* buffer, tFormat format, va_list* args);
int out_string(char* buffer, const tFormat format, const char* str);
s21_size_t out_integer(char* buffer, const tFormat format,
                       const unsigned long number);
s21_size_t get_prefix_length(tFormat tFormat, const unsigned long number);
int format_with_precision(char* numbuff, tFormat format);
int set_prefix(char* numbuff, tFormat format, const unsigned long number);
int justify_formatted_string(char* buffer, const tFormat format,
                             const char* str, const s21_size_t length);
s21_size_t out_double(char* buffer, tFormat format, const long double number);
s21_size_t s21_n_or_f_toa(char* numbuff, long double number, tFormat format,
                          int* type_spec_store);

s21_size_t s21_itoa(char* buffer, const unsigned long number, const int base);
s21_size_t s21_ftoa(char* buffer, const long double number, tFormat format,
                    unsigned flag_g);
s21_size_t s21_ntoa(char* buffer, const long double number, tFormat format,
                    unsigned flag_g);

#endif