#include "builtins.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

const builtins kysh_builtins[] = {
    {"cd", kysh_cd}, {"help", kysh_help}, {"exit", kysh_exit}};

int kysh_num_builtins() {
  return sizeof(kysh_builtins) / sizeof(kysh_builtins[0]);
}

int kysh_cd(char **args) {
  if (args[1] == NULL) {
    fprintf(stderr, "kysh: cd: expected argument\n");
  } else {
    if (chdir(args[1]) != 0) {
      perror("kysh\n");
    }
  }
  return 1;
}

int kysh_help(char **args) {
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

int kysh_exit(char **args) { return EXIT_SUCCESS; }