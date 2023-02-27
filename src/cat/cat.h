#ifndef SRC_CAT_MODULE_H_
#define SRC_CAT_MODULE_H_

#include <stdio.h>
#include <string.h>

struct Flags {
  int b;
  int e;
  int n;
  int s;
  int t;
  int v;
};

void openfile(char *argv, struct Flags flag);
void find_flags(int argc, struct Flags *flag, char **argv, int *error,
                int *num_flags);

#endif  // SRC_CAT_MODULE_H_
