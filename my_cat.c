#include "my_cat.h"


int main(int argc, char **argv) {
  struct options option = {0};
  char *filenames[argc - 1];
  int error = 0, count_files = 0;

  for (int i = 1; i < argc && !error; i++) {
    if (argv[i][0] == '-')
      error = parse_argument(argv[i], &option);
    else
      filenames[count_files++] = argv[i];
  }

  if (!error) output(&option, filenames, count_files);

  return 0;
}


int parse_argument(char *argument, struct options *option) {
  int error = 0;
  if (argument[1] == '-') {
    if (!strcmp(argument, "--number-nonblank"))
      option->b = 1;
    else if (!strcmp(argument, "--number"))
      option->n = 1;
    else if (!strcmp(argument, "--squeeze-blank"))
      option->s = 1;
    else {
      print_error_message(argument + 2, no_gnu_flag);
      error = 1;
    }
  } else {
    for (int i = 1; i < (int)strlen(argument) && !error; i++) {
      switch (argument[i]) {
        case 'b':
          option->b = 1;
          break;
        case 'e':
          option->e = 1, option->v = 1;
          break;
        case 'n':
          option->n = 1;
          break;
        case 's':
          option->s = 1;
          break;
        case 't':
          option->t = 1;
          option->v = 1;
          break;
        case 'v':
          option->v = 1;
          break;
        case 'E':
          option->e = 1;
          break;
        case 'T':
          option->t = 1;
          break;
        default:
          print_error_message(argument + i, no_flag);
          error = 1;
          break;
      }
    }
  }
  return error;
}


void output(struct options *option, char *filenames[], int count_files) {
  FILE *p_file;
  for (int i = 0; i < count_files; i++) {
    if ((p_file = fopen(filenames[i], "r")) != NULL) {
      read_file(option, p_file);
      fclose(p_file);
    } else
      print_error_message(filenames[i], no_file);
  }
}

void read_file(struct options *option, FILE *p_file) {
  char c, prev_c = -1, last_c = '\n';
  int line_number = 1;
  while ((c = fgetc(p_file)) != EOF) {
    if (prev_c == '\n' && last_c == '\n' && c == '\n' && option->s) continue;
    if (last_c == '\n' &&
        ((option->n && !option->b) || (option->b && c != '\n')))
      printf("%6d\t", line_number++);
    prev_c = last_c;
    last_c = c;
    if (c == '\n' && option->e) putchar('$');
    if (c == '\t' && option->t) {
      printf("^I");
      continue;
    }
    if (option->v && c & 0b10000000) {
      printf("M-");
      c = c & 0b01111111;
    }
    if (option->v &&
        ((c < 32 && c != '\n' && c != '\t') || (c == 127 && (c = -1))))
      printf("^%c", c + 64);
    else
      putchar(c);
  }
}

void print_error_message(char *illegal_name, enum error_type err_type) {
  if (err_type == no_file)
    fprintf(stderr, "my_cat: %s: No such file or directory\n", illegal_name);
  else if (err_type == no_gnu_flag) {
    fprintf(stderr, "my_cat: illegal GNU option -- %s\n", illegal_name);
    puts("usage: my_cat [-benstv] [file ...]");
  } else if (err_type == no_flag) {
    fprintf(stderr, "my_cat: illegal option -- %c\n", *illegal_name);
    puts("usage: my_cat [-benstv] [file ...]");
  }
}
