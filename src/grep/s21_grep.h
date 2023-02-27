#ifndef SRC_S21_GREP_MODULE_H_
#define SRC_S21_GREP_MODULE_H_

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Flags {
  int e;
  int i;
  int v;
  int c;
  int l;
  int n;
  int h;
  int s;
  int f;
  int o;
};

void main_func(int argc, char **argv);

void parser(int argc, struct Flags *flag, char **argv, int *error,
            int *num_files, char **pattern, int *num_pattern, int *REG_FLAG);
void find_flags(struct Flags *flag, char **argv, int count, int argc,
                int *error, int i, char **pattern, int *num_pattern);
void add_pattern(char **pattern, int *num_pattern, char **argv, char end_str,
                 int count, int i);
void sdvig(char **argv, size_t len, int i, int count);

void check_pattern(int num_pattern, char **pattern, int *error,
                   struct Flags flag);
void openfile(char **argv, int file_i, struct Flags flag, int num_files,
              int *error, char **pattern, int num_pattern, int REG_FLAG);
void without_o(int *i, int num_pattern, char *line_f, char *line, int REG_FLAG,
               int * true, struct Flags flag, int *write);
void with_o(char *line_f, int REG_FLAG, char **s, int num_files,
            struct Flags flag, char **argv, int file_i, int *line_check,
            int num_line, int *i, int num_pattern, int line_counter,
            char **pattern, int check);
void req_o(int i, int num_pattern, int REG_FLAG, char *next_s, int *line_check,
           struct Flags flag, int num_files, char **argv, int file_i,
           int num_line, int line_counter, char **pattern);
void print_o(char *s, regmatch_t pmatch, int num_files, struct Flags flag,
             char **argv, int file_i, int *line_check, int num_line, int lenth,
             char *next);
void flag_c(struct Flags flag, int *error, int num_files, char **argv,
            int file_i, int success_counter);
void write_line(int *write, struct Flags flag, int *success_counter,
                char **argv, int file_i, int *stop, int num_files, int num_line,
                char *line);

#endif  // SRC_S21_GREP_MODULE_H_
