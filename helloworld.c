// Example of C programming.
// Author: K. Walsh
// Date: 3 January 2018

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

// readline() reads and returns one line of text from the given file descriptor,
// stopping at the first newline or error. The trailing newline is discarded. It
// can only handle lines with 255 characters or less. Upon error, an error
// message is printed and, if no data has been read at all, then NULL is
// returned. Otherwise, it whatever text has already been read so far. The
// returned string was allocated using malloc(), and the caller is responsible
// for calling free() to deallocate the string.
char *readline(int fd) {
    char *s = malloc(256); // assumes line will be 255 characters or less, plus zero terminator
    int n = 0;
    while (1) {
        int r = read(fd, s + n, 1);
        if (r == 0 && n == 0) {
            printf("There is no data available in file %d\n", fd);
            free(s);
            return NULL;
        } else if (r == 0) {
            printf("Line began but didn't end, and there is no more data available in file %d\n", fd);
            break;
        } else if (r < 0) {
            printf("There was a problem reading from file %d: error number %d\n", fd, n);
            break;
        } else if (s[n] == '\n') {
            break;
        } else if (n == 255) {
            printf("Already read 255 characters, but line didn't end yet in file %d\n", fd);
        } else { 
            n++;
        }
    }
    s[n] = '\0'; // add the terminating zero and return the string
    return s;
}

int main(int ac, char **av) {
    setbuf(stdout, NULL);

    printf("Hello, World!\n");

    if (ac == 1) {
        char *arg;
        arg = av[0];
        printf("Only one command line argument (%s) was given\n", arg);
    } else {
        printf("%d command line arguments were given:\n", ac);
        int i;
        for (i = 0; i < ac; i++) {
            char *arg;
            arg = av[i];
            printf("argument %d is: %s\n", i, arg);
        }
    }

    printf("Please type something: ");
    char *line;
    line = readline(0); // read from standard input
    if (line == NULL) {
      printf("Input error, sorry.\n");
      return 0;
    }

    int n = strlen(line);
    printf("You typed %d characters: %s\n", n, line);
    free(line);

    printf("Bye!\n");
    return 0;
}
