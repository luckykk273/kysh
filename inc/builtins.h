/*
 * Builtin shell commands
 */
#ifndef BUILTINS_H
#define BUILTINS_H

#ifdef __cplusplus
extern "C" {
#endif

typedef int (*builtin_func)(char **args);
typedef struct {
  const char *name;
  builtin_func func;
} builtins;

extern const builtins kysh_builtins[];

int kysh_cd(char **args);
int kysh_help(char **args);
int kysh_exit(char **args);
int kysh_num_builtins(void);

#ifdef __cplusplus
}
#endif

#endif  // BUILTINS_H
