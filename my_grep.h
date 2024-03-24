#ifndef S21_GREP
#define S21_GREP

#define BUF_SIZE 4096

#include <regex.h>
#include <bits/getopt_core.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>


struct options {
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

void set_option(int, struct options *option);
void output(struct options *, char *[], int, char *[], int);
int read_file(FILE *, char *, regex_t *, int, int, struct options *);
int find_match(int, char *, int *, regex_t *);

#endif