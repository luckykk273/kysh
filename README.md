# kysh
A simple shell in C.

## Preface
This repository was originally created by following the [website tutorial](https://brennan.io/2015/01/16/write-a-shell-in-c/), and later had some additional features added(e.g. tokenizer, multiple pipes and I/O redirection). There are some known errors, but in most cases, it runs well enough. Since this repo is just intended to help with understanding some important [concepts](#key-concepts), these errors **WON'T** be fixed for now.

## Key Concepts
1. How a simple shell work
2. How to use the system call in C
    - pipes (`fork()`, `pipe()`, `execvp()`, `close()`, ...)
3. How to tokenize the command line
4. Linked list (`command_t`)

## Tests
```bash
kysh> ls -al | rev | nl | cat -e
```

## TODO
1. I/O redirection

## Reference
1. [Tutorial - Write a Shell in C](https://brennan.io/2015/01/16/write-a-shell-in-c/)
2. [GitHub - lsh](https://github.com/brenns10/lsh)
3. [GitHub Gist - pipex.c](https://gist.github.com/iomonad/a66f6e9cfb935dc12c0244c1e48db5c8)
4. [CS 702 - Operating Systems - Spring 2005 - Using dup2 for I/O Redirection and Pipes](https://www.cs.loyola.edu/~jglenn/702/S2005/Examples/dup2.html)
