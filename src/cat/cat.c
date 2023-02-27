#include "cat.h"

struct Flags;

int main(int argc, char *argv[]) {
  if (argc > 1) {
    int error = 0, num_flags = 0;
    struct Flags flag = {0, 0, 0, 0, 0, 0};
    find_flags(argc, &flag, argv, &error, &num_flags);
    if (error != 1) {
      for (int i = num_flags; i < argc; i++) {
        openfile(argv[i], flag);
      }
    } else {
      fprintf(stderr, "No such flag");
    }
  }
  return 0;
}

void find_flags(int argc, struct Flags *flag, char **argv, int *error,
                int *num_flags) {
  int count = 1;
  while (count < argc && argv[count][0] == '-' && *error != 1) {
    if (argv[count][1] == '-' || argv[count][1] == 'E' ||
        argv[count][1] == 'T') {
      if (!strcmp(argv[count], "--number-nonblank")) {
        (*flag).b = 1;
      } else if (!strcmp(argv[count], "-E")) {
        (*flag).e = 1;
      } else if (!strcmp(argv[count], "--number")) {
        (*flag).n = 1;
      } else if (!strcmp(argv[count], "--squeeze-blank")) {
        (*flag).s = 1;
      } else if (!strcmp(argv[count], "-T")) {
        (*flag).t = 1;
      } else {
        *error = 1;
      }
    } else {
      for (size_t i = 1; i < strlen(argv[count]); i++) {
        if (argv[count][i] == 'b') {
          (*flag).b = 1;
        } else if (argv[count][i] == 'e') {
          (*flag).e = 1;
          (*flag).v = 1;
        } else if (argv[count][i] == 'n') {
          (*flag).n = 1;
        } else if (argv[count][i] == 's') {
          (*flag).s = 1;
        } else if (argv[count][i] == 't') {
          (*flag).t = 1;
          (*flag).v = 1;
        } else if (argv[count][i] == 'v') {
          (*flag).v = 1;
        } else {
          *error = 1;
        }
      }
    }
    count++;
  }
  *num_flags = count;
}

void openfile(char *argv, struct Flags flag) {
  FILE *f;
  char read;
  if ((f = fopen(argv, "r")) == NULL) {
    fprintf(stderr, "cat: %s: No such file or directory\n", argv);
  } else {
    int empty_check = 0, str_count = 1, counter = 0;
    char check_s = '0';
    while (fscanf(f, "%c", &read) != EOF) {
      if ((flag.s == 0) ||
          (!(read == '\n' && read == check_s) || counter < 2)) {
        if ((flag.b == 0 && flag.n == 1 && empty_check == 0) ||
            (flag.b == 1 && (empty_check == 0 && read != '\n'))) {
          empty_check = 1;
          printf("%6d\t", str_count);
          str_count++;
        }
        if (read == '\n') {
          empty_check = 0;
          if (flag.e == 1) {
            printf("$");
          }
        }
        if (flag.v == 1 && read != '\t' && read < ' ' && read != '\n') {
          printf("^%c", read + 64);
        } else if (flag.t == 1 && read < ' ' && read != '\n') {
          printf("^%c", read + 64);
        } else {
          if (read >= 127 && flag.v == 1) {
            if (read == 127) {
              printf("^?");
            } else {
              printf("M-");
            }
          } else {
            printf("%c", read);
          }
        }
        if (read == '\n') {
          counter++;
          check_s = '\n';
        } else {
          counter = 0;
          check_s = '0';
        }
      }
    }
    fclose(f);
  }
}
