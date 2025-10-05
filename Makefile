OS= $(shell uname)
ifeq ("$(OS)","Linux")
	LEAKS_RUN_TEST = valgrind --tool=memcheck --leak-check=full ./test 
else
	LEAKS_RUN_TEST = leaks -atExit-- ./test
endif

CC=gcc
CFLAGS=-std=c11 -Wall -Werror -Wextra -pedantic
CFLAGS_TEST= -std=c11 -Wall -Werror -Wextra -pedantic
GCOVFLAGS = --coverage

CLANG_FORMAT=../materials/linters/.clang-format

ifeq ($(shell uname -p), arm)
	CC = gcc-14
endif

SRC=s21_memchr.c s21_memcmp.c s21_memcpy.c s21_memset.c s21_strchr.c s21_strcspn.c s21_strerror.c $\
s21_strlen.c s21_strncat.c s21_strncmp.c s21_strncpy.c s21_strpbrk.c s21_strrchr.c s21_strstr.c s21_strtok.c $\
s21_to_upper.c s21_to_lower.c s21_trim.c s21_insert.c s21_sprintf.c s21_sscanf.c

OBJECTS = $(addprefix obj/,$(SRC:.c=.o))
OBJECTS_GCOV = $(addprefix gcov_obj/,$(SRC:.c=.o))

FREE_TEST=rm -rf test.c test.o

all: s21_string.a
	
clean:
	rm -rf *.o *.so *.a *.out *.gcda *.gcno *.info test test.c test_gcov report obj gcov_obj

rebuild: clean all

s21_string.a: $(OBJECTS)
	ar rc s21_string.a $(OBJECTS)
	ranlib s21_string.a

s21_string_gcov.a: $(OBJECTS_GCOV)
	ar rc s21_string_gcov.a $(OBJECTS_GCOV)
	ranlib s21_string_gcov.a

test.c: test.check
	checkmk clean_mode=1 test.check sprintf.check sscanf.check >test.c	

test.o: test.c s21_string.h
	$(CC) $(CFLAGS_TEST) -g -c test.c	

test: clean test.o s21_string.a	
	$(CC) $(CFLAGS) -g test.o -o test `pkg-config --cflags --libs check` -L. s21_string.a -lm
	./test
	$(FREE_TEST)

leak_test: clean test.o s21_string.a
	$(CC) $(CFLAGS) -g test.o -o test `pkg-config --cflags --libs check` -L. s21_string.a -lm
	$(LEAKS_RUN_TEST)
	$(FREE_TEST)	

gcov_report: clean test.o s21_string_gcov.a
	$(CC) $(CFLAGS) test.o -o gcov_obj/test_gcov `pkg-config --cflags --libs check` -L. s21_string_gcov.a $(GCOVFLAGS) -lm
	gcov_obj/test_gcov
	mkdir -p report	
	gcovr --html-details report/index.html --html-medium-threshold 80 --html-high-threshold 100 --html-title "Coverage s21_string+ report"
	$(FREE_TEST)

obj/%.o: %.c
	mkdir -p obj/
	$(CC) $(CFLAGS) -g -c $< -o $@

gcov_obj/%.o: %.c
	mkdir -p gcov_obj
	$(CC) $(CFLAGS) $(GCOVFLAGS) -c $< -o $@

clang:
	clang-format --style=file:$(CLANG_FORMAT) -n *.c *.h
	clang-format --style=file:$(CLANG_FORMAT) -i *.c *.h

check:
	cppcheck -q --enable=warning,portability --inconclusive ./

.PHONY: all clean rebuild clang check