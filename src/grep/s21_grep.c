#include "s21_grep.h"

struct Flags;

int main(int argc, char *argv[]) {
  if (argc > 1) {
    main_func(argc, argv);
  }
  return 0;
}

void main_func(int argc, char **argv) {
  int error = 0, num_pattern = 0, num_files = 0, REG_FLAG = REG_EXTENDED;
  size_t max_len = 0;
  for (int i = 1; i < argc; i++) {
    if (max_len < strlen(argv[i])) {
      max_len = strlen(argv[i]);
    }
  }
  char **pattern = malloc(argc * sizeof(char *));
  for (int i = 0; i < argc; i++) {
    pattern[i] = malloc((max_len + 2) * sizeof(char));
    pattern[i][0] = '\0';
  }
  struct Flags flag = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  parser(argc, &flag, argv, &error, &num_files, pattern, &num_pattern,
         &REG_FLAG);
  if (flag.e == 0 && flag.f == 0 && num_pattern == 0) {
    int k = 1;
    while (argv[k][0] == '\0' && argv[k][1]) {
      k++;
    }
    add_pattern(pattern, &num_pattern, argv, '\0', k, 0);
  }
  if (flag.e == 0 && flag.f == 0 && argv[1][0] == '\0') {
    num_files--;
  }
  if (error != 1 || num_files == 0) {
    int check_start = 1;
    for (int i = 1; i < argc; i++) {
      if (flag.e == 0 && flag.f == 0 && check_start == 1) {
        i++;
        check_start--;
      }
      if (i < argc && argv[i][0] != '\0') {
        openfile(argv, i, flag, num_files, &error, pattern, num_pattern,
                 REG_FLAG);
      }
    }
  } else {
    fprintf(stderr, "Flag error\n");
  }
  for (int i = 0; i < argc; i++) {
    free(pattern[i]);
  }
  free(pattern);
}

void parser(int argc, struct Flags *flag, char **argv, int *error,
            int *num_files, char **pattern, int *num_pattern, int *REG_FLAG) {
  int count = 1;
  size_t len_str;
  while (count < argc && *error != 1) {
    len_str = strlen(argv[count]);
    if (len_str == 0) {
      add_pattern(pattern, num_pattern, argv, '\0', 0, 0);
    }
    for (int i = 1; (size_t)i < len_str; i++) {
      if (argv[count][0] == '-') {
        find_flags(flag, argv, count, argc, error, i, pattern, num_pattern);
        if (!(argv[count][i + 1])) {
          argv[count][0] = '\0';
        }
        if (argv[count][0] == '\0' && argv[count][i] == 'e' &&
            count < argc - 2 && argv[count + 2][0] != '-') {
          add_pattern(pattern, num_pattern, argv, '\0', count + 1, 0);
          count = count + 2;
          *num_files = *num_files + 1;
        } else if (argv[count][0] == '\0' && argv[count][i] == 'e' &&
                   count < argc - 1) {
          add_pattern(pattern, num_pattern, argv, '\0', count + 1, 0);
          count = count + 1;
        } else if (argv[count][0] == '\0' && argv[count][i] == 'f' &&
                   count < argc - 1) {
          add_pattern(pattern, num_pattern, argv, '\n', count + 1, 0);
          count = count + 1;
        }
      } else if (i == 1) {
        *num_files = *num_files + 1;
      }
    }
    count++;
  }
  if ((*flag).v == 1 || (*flag).c == 1 || (*flag).l == 1) {
    (*flag).o = 0;
  }
  if ((*flag).i) {
    *REG_FLAG |= REG_ICASE;
  }
}

void find_flags(struct Flags *flag, char **argv, int count, int argc,
                int *error, int i, char **pattern, int *num_pattern) {
  if (argv[count][i] == 'i') {
    (*flag).i = 1;
  } else if (argv[count][i] == 'v') {
    (*flag).v = 1;
  } else if (argv[count][i] == 'c') {
    (*flag).c = 1;
  } else if (argv[count][i] == 'l') {
    (*flag).l = 1;
  } else if (argv[count][i] == 'n') {
    (*flag).n = 1;
  } else if (argv[count][i] == 'h') {
    (*flag).h = 1;
  } else if (argv[count][i] == 's') {
    (*flag).s = 1;
  } else if (argv[count][i] == 'o') {
    (*flag).o = 1;
  } else if (argv[count][i] == 'e') {
    (*flag).e++;
    if (count == argc - 1) {
      *error = 1;
    }
    size_t len = strlen(argv[count]);
    if ((size_t)(i + 1) < len) {
      sdvig(argv, len, i, count);
      add_pattern(pattern, num_pattern, argv, '\0', count, i);
    }
  } else if (argv[count][i] == 'f') {
    (*flag).f++;
    if (count == argc - 1) {
      *error = 1;
    }
    size_t len = strlen(argv[count]);
    if ((size_t)(i + 1) < len) {
      sdvig(argv, len, i, count);
      add_pattern(pattern, num_pattern, argv, '\n', count, i);
    }
  } else {
    *error = 1;
  }
}

void sdvig(char **argv, size_t len, int i, int count) {
  for (size_t j = 0, k = i + 1; k < len; k++, j++) {
    argv[count][j] = argv[count][k];
    argv[count][k] = '\0';
  }
}

void add_pattern(char **pattern, int *num_pattern, char **argv, char end_str,
                 int count, int i) {
  strcpy(pattern[*num_pattern], argv[count]);
  pattern[*num_pattern][strlen(argv[count])] = '\0';
  pattern[*num_pattern][strlen(argv[count]) + 1] = end_str;
  *num_pattern = *num_pattern + 1;
  argv[count][0] = '\0';
  argv[count][i] = '\0';
}

void check_pattern(int num_pattern, char **pattern, int *error,
                   struct Flags flag) {
  char *line_f = NULL;
  int r = 0;
  size_t len = 0;
  regex_t regex;
  for (int i = 0; i < num_pattern; i++) {
    if (i < num_pattern && pattern[i][strlen(pattern[i]) + 1] == '\n') {
      FILE *f_flag;
      char f_read;
      if ((f_flag = fopen(pattern[i], "r")) == NULL) {
        if (flag.s == 0) {
          fprintf(stderr, "grep: %s: No such file or directory\n", pattern[i]);
        }
      } else {
        while ((f_read = getline(&line_f, &len, f_flag)) != EOF &&
               *error == 0) {
          if (i < num_pattern) {
            r = regcomp(&regex, line_f, 0);
            regfree(&regex);
            if (r) {
              fprintf(stderr, "Regex error\n");
              *error = 1;
              i = num_pattern;
            }
          }
        }
      }
      free(line_f);
      fclose(f_flag);
    } else {
      if (pattern[i][0] != '\0') {
        r = regcomp(&regex, pattern[i], 0);
        regfree(&regex);
      }
      if (r) {
        fprintf(stderr, "Regex error\n");
        *error = 1;
        i = num_pattern;
      }
    }
  }
}

void without_o(int *i, int num_pattern, char *line_f, char *line, int REG_FLAG,
               int * true, struct Flags flag, int *write) {
  regex_t regex;
  if (*i < num_pattern) {
    regcomp(&regex, line_f, REG_FLAG);
    *true = regexec(&regex, line, 0, NULL, 0);
    if ((!(regexec(&regex, ".", 0, NULL, 0))) && line[0] == '\n') {
      *true = 1;
    }
    regfree(&regex);
  }
  if (line_f[0] == '\0') {
    *true = 0;
  }
  if (*true == 0 && flag.v == 0) {
    *write = 1;
    *i = num_pattern;
  }
  if (*true == REG_NOMATCH && flag.v == 1) {
    *write = 1;
  } else if (*true == 0 && flag.v == 1) {
    *write = 0;
    *i = num_pattern;
  }
}

void req_o(int i, int num_pattern, int REG_FLAG, char *next_s, int *line_check,
           struct Flags flag, int num_files, char **argv, int file_i,
           int num_line, int line_counter, char **pattern) {
  char *s = next_s, *line_new = NULL, f_read;
  int next_line = line_counter;
  size_t len_f = 0;
  FILE *f_flag;
  for (int j = i + 1; j < num_pattern; j++) {
    if (j < num_pattern && pattern[j][strlen(pattern[j]) + 1] == '\n') {
      f_flag = fopen(pattern[j], "r");
      if (f_flag != NULL) {
        while (line_counter > 1 &&
               (f_read = getline(&line_new, &len_f, f_flag)) != EOF) {
          line_counter--;
        }
        while ((f_read = getline(&line_new, &len_f, f_flag)) != EOF) {
          if (line_new[strlen(line_new) - 1] == '\n') {
            line_new[strlen(line_new) - 1] = '\0';
          }
          with_o(line_new, REG_FLAG, &s, num_files, flag, argv, file_i,
                 line_check, num_line, &j, num_pattern, next_line, pattern, 0);
        }
        free(line_new);
        fclose(f_flag);
      }
    } else {
      with_o(pattern[j], REG_FLAG, &s, num_files, flag, argv, file_i,
             line_check, num_line, &j, num_pattern, 1, pattern, 0);
    }
  }
}

void with_o(char *line_f, int REG_FLAG, char **s, int num_files,
            struct Flags flag, char **argv, int file_i, int *line_check,
            int num_line, int *i, int num_pattern, int line_counter,
            char **pattern, int check) {
  regex_t regex;
  regmatch_t pmatch[1];
  regcomp(&regex, line_f, REG_FLAG);
  int lenth, end = 1;
  if (!(regexec(&regex, *s, 0, NULL, 0)) || line_f[0] == '\0') {
    end = 0;
  }
  if (end == 0) {
    regexec(&regex, *s, 1, pmatch, 0);
    lenth = pmatch[0].rm_eo - pmatch[0].rm_so;
    char next[lenth];
    next[0] = '\0';
    print_o(*s, pmatch[0], num_files, flag, argv, file_i, line_check, num_line,
            lenth, next);
    *s += pmatch[0].rm_eo;
    req_o(*i, num_pattern, REG_FLAG, next, line_check, flag, num_files, argv,
          file_i, num_line, line_counter, pattern);
    if (check == 1) {
      *i = -1;
    }
  }
  regfree(&regex);
}

void print_o(char *s, regmatch_t pmatch, int num_files, struct Flags flag,
             char **argv, int file_i, int *line_check, int num_line, int lenth,
             char *next) {
  if (num_files > 1 && flag.h == 0) {
    if (*line_check == 1) {
      printf("%s:", argv[file_i]);
      if (!flag.n) {
        *line_check = 0;
      }
    }
    if (flag.n == 1 && *line_check == 1) {
      printf("%d:", num_line);
      *line_check = 0;
    }
    printf("%.*s\n", lenth, s + pmatch.rm_so);
  } else {
    if (flag.n == 1 && *line_check == 1) {
      printf("%d:", num_line);
      *line_check = 0;
    }
    printf("%.*s\n", lenth, s + pmatch.rm_so);
  }
  s += pmatch.rm_eo;
  strncpy(next, s - pmatch.rm_eo + pmatch.rm_so, lenth);
  next[lenth] = '\0';
}

void openfile(char **argv, int file_i, struct Flags flag, int num_files,
              int *error, char **pattern, int num_pattern, int REG_FLAG) {
  FILE *f;
  char read;
  int success_counter = 0;
  if ((f = fopen(argv[file_i], "r")) == NULL) {
    if (flag.s == 0) {
      fprintf(stderr, "grep: %s: No such file or directory\n", argv[file_i]);
    }
  } else {
    int num_line = 0, write = 0, true, stop = 0;
    char *line = NULL;
    size_t len = 0, len_f = 0;
    check_pattern(num_pattern, pattern, error, flag);
    while ((read = getline(&line, &len, f)) != EOF && *error == 0 &&
           stop == 0) {
      num_line++;
      int line_check = 1;
      char *s = line;
      for (int i = 0; i < num_pattern; i++) {
        if (i < num_pattern && pattern[i][strlen(pattern[i]) + 1] == '\n') {
          FILE *f_flag;
          char f_read, *line_f = NULL;
          int line_counter = 0;

          if ((f_flag = fopen(pattern[i], "r")) != NULL) {
            while ((f_read = getline(&line_f, &len_f, f_flag)) != EOF) {
              line_counter++;
              if (line_f[strlen(line_f) - 1] == '\n') {
                line_f[strlen(line_f) - 1] = '\0';
              }
              if (flag.o == 0) {
                without_o(&i, num_pattern, line_f, line, REG_FLAG, &true, flag,
                          &write);
              } else {
                with_o(line_f, REG_FLAG, &s, num_files, flag, argv, file_i,
                       &line_check, num_line, &i, num_pattern, line_counter,
                       pattern, 1);
              }
            }
            free(line_f);
            fclose(f_flag);
          }
        } else {
          if (flag.o == 0) {
            without_o(&i, num_pattern, pattern[i], line, REG_FLAG, &true, flag,
                      &write);
          } else {
            with_o(pattern[i], REG_FLAG, &s, num_files, flag, argv, file_i,
                   &line_check, num_line, &i, num_pattern, 1, pattern, 1);
          }
        }
      }
      write_line(&write, flag, &success_counter, argv, file_i, &stop, num_files,
                 num_line, line);
    }
    free(line);
    flag_c(flag, error, num_files, argv, file_i, success_counter);
    fclose(f);
  }
}

void write_line(int *write, struct Flags flag, int *success_counter,
                char **argv, int file_i, int *stop, int num_files, int num_line,
                char *line) {
  if (*write == 1) {
    if (flag.c == 1 && flag.l == 0) {
      *success_counter = *success_counter + 1;
    } else if (flag.l == 1) {
      *success_counter = *success_counter + 1;
      if (!flag.c) {
        printf("%s\n", argv[file_i]);
      }
      *stop = 1;
    } else {
      if (num_files > 1 && flag.h == 0) {
        printf("%s:", argv[file_i]);
        if (flag.n == 1) {
          printf("%d:", num_line);
        }
        printf("%s", line);
      } else {
        if (flag.n == 1) {
          printf("%d:", num_line);
        }
        printf("%s", line);
      }
      if (line[strlen(line) - 1] != '\n') {
        printf("\n");
      }
    }
  }
  *write = 0;
}

void flag_c(struct Flags flag, int *error, int num_files, char **argv,
            int file_i, int success_counter) {
  if (flag.c == 1 && *error == 0) {
    if (num_files > 1 && flag.h == 0) {
      printf("%s:", argv[file_i]);
    }
    printf("%d\n", success_counter);
    if (flag.l && success_counter != 0) {
      printf("%s\n", argv[file_i]);
    }
  }
}
