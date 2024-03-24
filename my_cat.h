#ifndef S21_CAT
#define S21_CAT

#include <stdio.h>
#include <string.h>

struct options {
  int b;
  int e;
  int n;
  int s;
  int t;
  int v;
  int E;
  int T;
};

enum error_type { no_file, no_flag, no_gnu_flag };

int parse_argument(char *, struct options *option);
void output(struct options *, char *[], int);
void read_file(struct options *, FILE *);
void print_error_message(char *, enum error_type);

#endif