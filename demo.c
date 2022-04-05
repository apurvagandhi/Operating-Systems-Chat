// demo - a simple interactive program
// Author: K. Walsh
// Date: 24 Dec 2017
// Compile using GNU's gcc compiler:
//   gcc -g -Wall -o demo demo.c
// Alternatively, compile using more friendly clang compilier:
//   clang -g -Wall -o demo demo.c
// Execute interactively:
//   ./demo
// Execute using a prepared input file:
//   ./demo < inputfile > outputfile
// Updates:
//   K. Walsh - 3 Jan 2022 - Added new readline function.
//   K. Walsh - 20 Jan 2022 - Converted to demo starter code.
//   [ TODO: add name, date, description of updates ]


#include <stdio.h> // for printf(), read(), etc.
#include <stdlib.h> // for malloc(), free(), etc.
#include <unistd.h> // for open(), close(), etc.
#include <string.h> // for string functions
#include <sys/types.h> // needed for other header files
#include <sys/resource.h> // for getrlimit()
#include <fcntl.h> // for fcntl() used by our own fstatus() function


// fstatus() prints a list of all currently open file descriptors, along with
// their mode (read-only, read-write, or write-only).
void fstatus() {
    // query the resource limit system to find NOFILE, the largest allowed fd
    struct rlimit rlim;
    if (getrlimit(RLIMIT_NOFILE, &rlim) == -1) {
        printf("Sorry, failed to query resource limits.\n");
        return;
    }
    int max_fd = rlim.rlim_cur - 1;
    // loop through every possible file descriptor number
    for (int fd = 0; fd <= max_fd; fd++) {
        // query file control system to see if file descriptor is valid
        if (fcntl(fd, F_GETFD) == -1)
            continue; // this fd value is not valid
        // query file control system to see if fd is readable, writable, etc.
        int mode = fcntl(fd, F_GETFL);
        if (mode == -1)
            printf(" fd %d is open, mode=unknown\n", fd);
        else if ((mode & O_ACCMODE) == O_RDONLY)
            printf(" fd %d is open, mode=RDONLY\n", fd);
        else if ((mode & O_ACCMODE) == O_WRONLY)
            printf(" fd %d is open, mode=WRONLY\n", fd);
        else if ((mode & O_ACCMODE) == O_RDWR)
            printf(" fd %d is open, mode=RDWR\n", fd);
        else
            printf(" fd %d is open, mode=other\n", fd);
    }
}


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

// main() contains the primary interaction loop
int main(int ac, char **av) {

    setbuf(stdout, NULL); // needed to avoid buffering output

    printf("Welcome to example program!\n");

    int quit = 0;
    while (!quit) {
        printf("Type 'quit', 'show file status', or 'hello': ");

        // Get the next line of input.
        char *cmd = readline(0);
        if (cmd == NULL) {
            printf("Oops, no more input available! Exiting...\n");
            break;
        }

        // Do something, depending on the user input.
        if (!strcmp(cmd, "hello")) {
            printf("Well hello to you too!\n");
        } else if (!strcmp(cmd, "show your name")) {
            printf("My name is Frank.\n");
        } else if (!strcmp(cmd, "how are you")) {
            printf("I am fine, thanks.\n");
        } else if (!strncmp(cmd, "show file status ", 5)) {
            fstatus();
        } else if (!strcmp(cmd, "quit")) {
            quit = 1;
        } else {
            printf("Sorry, I don't know how to do that.\n");
        }

        // Free the command string, so we don't leak memory.
        free(cmd);
    }

    printf("Bye bye now!\n");
    return 0;
}
