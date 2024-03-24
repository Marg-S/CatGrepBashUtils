#include "my_grep.h"

int main(int argc, char **argv) {
  struct options option = {0};
  char *filenames[argc - 1];
  char *patterns[argc - 1];
  char *flags = "e:ivclnhsf:o";
  int error = 0, count_files = 0, count_patterns = 0, opt = -1;

  while (optind < argc && !error) {
    opt = getopt(argc, argv, flags);
    if (opt == '?')
      error = 1;
    else if (opt == -1) {
      filenames[count_files++] = argv[optind];
      optind++;
    } else if (opt == 'e') {
      patterns[count_patterns++] = optarg;
    } else if (opt == 'f') {
      FILE *p_file = fopen(optarg, "r");
      if (!p_file) {
        if (!option.s)
          fprintf(stderr, "my_grep: %s: No such file or directory\n", optarg);
        error++;
        break;
      }
      char buffer[BUF_SIZE];
      while (fgets(buffer, sizeof(buffer), p_file)) {
        patterns[count_patterns++] = buffer;
      }

    } else
      set_option(opt, &option);
  }

  if (!count_files || (count_files == 1 && !count_patterns)) {
    fprintf(stderr,
            "usage: my_grep [-ivclnhso] [-e pattern] [-f file] [pattern] [file "
            "...]\n");
    error++;
  }
  if (!error) output(&option, filenames, count_files, patterns, count_patterns);

  return 0;
}

void set_option(int argument, struct options *option) {
  switch (argument) {
    case 'i':
      option->i = 1;
      break;
    case 'v':
      option->v = 1;
      break;
    case 'c':
      option->c = 1;
      break;
    case 'l':
      option->l = 1;
      break;
    case 'n':
      option->n = 1;
      break;
    case 'h':
      option->h = 1;
      break;
    case 's':
      option->s = 1;
      break;
    case 'f':
      option->f = 1;
      break;
    case 'o':
      option->o = 1;
      break;
    default:
      puts("error");
      break;
  }
}

void output(struct options *option, char *filenames[], int count_files,
            char *patterns[], int count_patterns) {
  FILE *p_file;
  int rerr = 0;
  int cflags = REG_NEWLINE;
  if (option->i) cflags = cflags | REG_ICASE;
  if (count_patterns == 0) {
    patterns[count_patterns++] = filenames[0];
    filenames++;
    count_files--;
  }

  regex_t preg[count_patterns];
  for (int i = 0; i < count_patterns && !rerr; i++) {
    if ((rerr = regcomp(&preg[i], patterns[i], cflags))) {
      char buffer[BUF_SIZE];
      regerror(rerr, &preg[i], buffer, sizeof(buffer));
      fprintf(stderr, "my_grep: %s\n", buffer);
    }
  }

  for (int i = 0; i < count_files && !rerr; i++) {
    if ((p_file = fopen(filenames[i], "r"))) {
      rerr = read_file(p_file, filenames[i], preg, count_files, count_patterns,
                       option);
      fclose(p_file);
    } else {
      if (!option->s)
        fprintf(stderr, "my_grep: %s: No such file or directory\n",
                filenames[i]);
    }
  }
  for (int i = 0; i < count_patterns; i++) regfree(&preg[i]);
}

int read_file(FILE *p_file, char *filename, regex_t *preg, int count_files,
              int count_patterns, struct options *option) {
  int rerr = 0, c_match = 0, fnd_match = 0, n_line = 0;
  char line[BUF_SIZE];
  while (fgets(line, sizeof(line), p_file) && !rerr &&
         !(c_match && option->l && !option->v)) {
    fnd_match = find_match(count_patterns, line, &c_match, preg);
    if (fnd_match == -1)
      rerr++;
    else if ((fnd_match && !option->v) || (!fnd_match && option->v)) {
      if (!option->l && !option->c) {
        if (count_files > 1 && !option->h) printf("%s:", filename);
        if (option->n) printf("%d:", n_line + 1);
        if (line[strlen(line) - 1] == '\n') line[strlen(line) - 1] = '\0';
        puts(line);
      }
    }
    n_line++;
  }
  if (option->c) {
    if (count_files > 1 && !option->h) printf("%s:", filename);
    if (option->v) {
      if (option->l && (c_match != n_line))
        printf("1\n");
      else
        printf("%d\n", n_line - c_match);
    } else
      printf("%d\n", c_match);
  }
  if (option->l &&
      ((c_match && !option->v) || (c_match != n_line && option->v)))
    printf("%s\n", filename);

  return rerr;
}

int find_match(int count_patterns, char *line, int *c_match, regex_t *preg) {
  int rerr = 0, fnd_match = 0;
  for (int i = 0; i < count_patterns && !fnd_match && !rerr; i++) {
    regmatch_t pmatch[preg[i].re_nsub + 1];
    if (!(rerr = regexec(&preg[i], line, preg[i].re_nsub + 1, pmatch, 0))) {
      fnd_match++;
      (*c_match)++;
    }
    if (rerr == REG_NOMATCH) rerr = 0;
    if (rerr) {
      char buffer[BUF_SIZE];
      regerror(rerr, &preg[i], buffer, sizeof(buffer));
      fprintf(stderr, "my_grep: %s\n", buffer);
      fnd_match = -1;
    }
  }
  return fnd_match;
}