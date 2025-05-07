/*
 * Builtin shell commands
 */
#ifndef BUILTINS_H
#define BUILTINS_H

#ifdef __cplusplus
extern "C" {
#endif

typedef int (*builtin_func_t)(char **argv);
typedef struct builtins {
  const char *name;
  builtin_func_t func;
} builtins_t;

extern const builtins_t kysh_builtins[];

int kysh_cd(char **argv);
int kysh_help(char **argv);
int kysh_exit(char **argv);
int kysh_num_builtins(void);
void kysh_display_cwd(void);

#ifdef __cplusplus
}
#endif

#endif  // BUILTINS_H
