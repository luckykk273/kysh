# kysh
A simple shell in C.

## Preface
This repository was originally created by following the [website tutorial](https://brennan.io/2015/01/16/write-a-shell-in-c/), and later had some additional features added (e.g. tokenizer, multiple pipes and I/O redirection). There are some known errors, but in most cases, it runs well enough. Since this repo is just intended to help with understanding some important [concepts](#key-concepts), these errors **WON'T** be fixed for now.

## Key Concepts
1. How a simple shell work
2. How to use the system call in C
    - pipes (`fork()`, `pipe()`, `execvp()`, `close()`, ...)
3. How to tokenize the command line
    - Tokenize by delimiters: `' '`, `'\t'`, `'\r'`, `'\n'`, `'\a'`
    - Handle special delimiters for pipes and I/O redirection: `'|'`, `'<'`, `'>'`
4. Linked list (`command_t`)

## Tests
Some cases are tested as below:
1. Multiple pipes only
```bash
kysh> ls -al | rev | nl | cat -e
```

2. Multiple pipes with builtin function
```bash
kysh> cd .. | echo hello | echo world
kysh> echo hello | cd .. | echo world
kysh> echo hello | echo world | cd ..
```

3. Error catch
```bash
kysh> cd..
kysh> cd.. | ls
```

4. Arbitrary whitespaces between commands and delimiters
```bash
kysh> echo hello     | ls
kysh> echo hello|      ls
kysh> ls     -al    |rev  | nl |cat    -e
```

## TODO
1. I/O redirection

## Reference
1. [Tutorial - Write a Shell in C](https://brennan.io/2015/01/16/write-a-shell-in-c/)
2. [GitHub - lsh](https://github.com/brenns10/lsh)
3. [GitHub Gist - pipex.c](https://gist.github.com/iomonad/a66f6e9cfb935dc12c0244c1e48db5c8)
4. [CS 702 - Operating Systems - Spring 2005 - Using dup2 for I/O Redirection and Pipes](https://www.cs.loyola.edu/~jglenn/702/S2005/Examples/dup2.html)
