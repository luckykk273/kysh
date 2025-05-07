#include "builtins.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define KYSH_DISPLAY_PATH_BUFSIZE (1024)

const builtins_t kysh_builtins[] = {
    {"cd", kysh_cd},
    {"help", kysh_help},
    {"exit", kysh_exit},
};

int kysh_num_builtins() {
  return sizeof(kysh_builtins) / sizeof(kysh_builtins[0]);
}

int kysh_cd(char **argv) {
  if (argv[1] == NULL) {
    fprintf(stderr, "kysh: cd: expected argument\n");
  } else {
    if (chdir(argv[1]) != 0) {
      perror("kysh: chdir");
    }
  }
  return 1;
}

int kysh_help(char **argv) {
  int i;
  printf("kysh\n");
  printf("Type program names and arguments, and hit enter.\n");
  printf("The following are built in:\n");

  for (i = 0; i < kysh_num_builtins(); i++) {
    printf("  %s\n", kysh_builtins[i].name);
  }

  printf("Use the man command for information on other programs.\n");
  return 1;
}

int kysh_exit(char **argv) { return EXIT_SUCCESS; }

void kysh_display_cwd(void) {
  char cwd[KYSH_DISPLAY_PATH_BUFSIZE];
  cwd[KYSH_DISPLAY_PATH_BUFSIZE - 1] = '\0';
  if (getcwd(cwd, KYSH_DISPLAY_PATH_BUFSIZE - 1) == NULL) {
    perror("kysh: getcwd");
  } else {
    printf("%s", cwd);
  }

  printf("> ");
}
