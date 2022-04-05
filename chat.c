/***********************************************************
* Name: Apurva Gandhi
* Instructor: Professor Walsh
* Date: February 2, 2022
* Course: CSCI346
* Assignment: Project0
* Description: A simple text-based assistant called chat. 
* It greets the user by name, then repeatedly prompt the user 
* for what to do next and respond to each request. A complete list 
* of commands can be accessed by typing "help"
************************************************************/

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h> //used to get the time
#include <string.h>
#include <limits.h>
#include <dirent.h>
#include <fcntl.h> // for fcntl() used by fstatus() function
#include <sys/resource.h> // for getrlimit()
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

/*  fstatus() prints a list of all currently open file descriptors, along with
    their mode (read-only, read-write, or write-only).
*/
void fstatus() 
{
    // query the resource limit system to find NOFILE, the largest allowed fd
    struct rlimit rlim;
    if (getrlimit(RLIMIT_NOFILE, &rlim) == -1) 
    {
        printf("Sorry, failed to query resource limits.\n");
        return;
    }
    int max_fd = rlim.rlim_cur - 1;
    // loop through every possible file descriptor number
    for (int fd = 0; fd <= max_fd; fd++) 
    {
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

/*  readline reads a single line of the given file descriptor
    it takes in file descriptor as parameter
    it returns the first line of the file or indicates error    
*/
char *readline(int fd) 
{
    char *s = malloc(256); // assumes line will be 255 characters or less, plus zero terminator
    int n = 0;
    while (1) 
    {
        int r = read(fd, s + n, 1);
        if (r == 0 && n == 0) 
        {
            printf("There is no data available in file %d\n", fd);
            free(s);
            return NULL;
        } else if (r == 0) 
        {
            printf("Line began but didn't end, and there is no more data available in file %d\n", fd);
            break;
        } else if (r < 0) 
        {
            printf("There was a problem reading from file %d: error number %d\n", fd, n);
            break;
        } else if (s[n] == '\n') 
        {
            break;
        } else if (n == 255) 
        {
            printf("Already read 255 characters, but line didn't end yet in file %d\n", fd);
            n++;
        } 
        else if( n > 255)
        {
            //this is for ignoring rest of the characters. 
            //without this condition, loop would print n==255 statement until all characters are read
        }
        else { 
            n++;
        }
    }
    s[n] = '\0'; // add the terminating zero and return the string
    return s;
}

/*  returns the current time in seconds if successful
    or returns -1 to indicate something went wrong
*/
int getCurrentTimeInSeconds()
{
    struct timespec now;
    if(clock_gettime(CLOCK_REALTIME, &now) == 0)
    {
        return now.tv_sec;
    }
    printf("Error encountered while getting current time in seconds.\n");
    return -1;
}

/*  returns the current time in nanoseconds if successful
    or returns -1 to indicate something went wrong
*/
int getCurrentTimeInNanoseconds()
{
    struct timespec now;
    if(clock_gettime(CLOCK_REALTIME, &now) == 0)
    {
        return now.tv_nsec;    
    }
    printf("Error encountered while getting current time in nanoseconds.\n");
    return -1;
}

/*  splitString takes the string and a character as parameter.
    It splits the original string by first occurance of a given character
    and returns string after the splitter character.
*/
char *splitString (char* original, char splitter)
{
    // strchr finds the first occurence of a space   
    char *newString = strchr(original, splitter);
    if(newString != NULL)
    {
        // turn the space into a '\0' "null character", effectively splitting (here assign using array notation )
        newString[0] = '\0';
        return newString+1;
    }
    return newString; //this will be a null String
}

int main(int ac, char **av) 
{
    setbuf(stdout, NULL);
    if(getenv("USER") != NULL)
    {
        printf("Welcome to chat assistant, %s!\n", getenv("USER"));
    }
    else
    {
        printf("Sorry, something went wrong while getting USER. USER might be null.\n");
    }

    int startTimeInSeconds = getCurrentTimeInSeconds();
    int startTimeInNanoseconds = getCurrentTimeInNanoseconds();
    //if the functions does not return error code, print the current/start time or print error message
    if(startTimeInSeconds != -1 && startTimeInNanoseconds != -1)
    {
        printf("The time is %d seconds and %d nanoseconds since midnight, Jan 1, 1970.\n", startTimeInSeconds, startTimeInNanoseconds);
    }
    else
    {
        printf("Error encountered while getting the current time.");
    }
    printf("Type \"help\" if you are lost.\n");

    int quit = 0;
    while (!quit) 
    {
        // Get the next line of input.
        printf("What next? ");
        char *cmd = readline(0);
        
        // Do something, depending on the user input.
        //If users types "help"
        if (!strcmp(cmd, "help")) 
        {
            printf("Here are the list of commands this program can respond:\n");
            printf("help\n");
            printf("how are you\n");
            printf("show the time\n");
            printf("show your name\n");
            printf("show your id\n");
            printf("show your parent's id\n");
            printf("show your age\n");
            printf("who am i\n");            
            printf("where am i\n");
            printf("say [any phrase]\n");
            printf("sleep [amount of time in seconds]\n");
            printf("list [name of directory]\n");
            printf("go to [name of directory]\n");
            printf("open [filename]\n");
            printf("read [file number]\n");
            printf("close [file number]\n");
            printf("show file status\n");
            printf("quit\n");
        } 
        //if users types "say [anything]", then print everything after space
        else if(!strncmp(cmd, "say ", 4))
        {
            printf("%s\n", splitString(cmd, ' '));
        }
        //programs sleeps for entered time by calling sleep function
        else if(!strncmp(cmd, "sleep ", 6))
        {
            // ascii-to-floatingpoint
            double sleepTime = atof(splitString(cmd, ' '));
            //error checking for time input
            if(sleepTime <= 0)
            {
                printf("Sorry, something went wrong with your input. \n");
                printf("Sleep time must be great 0 and time must be in numerical format\n");
            }
            else
            {
                printf("Going to sleep for %f seconds\n", sleepTime);
                // *1000,000 to convert from microseconds to seconds
                if(usleep(sleepTime*1000000) == 0)
                {            
                    printf("Ok, that was a nice nap.\n");
                } 
                else
                {
                    printf("Sorry, something went wrong while while executing sleep.\n");
                }
            }
            
        }
        //when user enters "how are you"
        else if (!strcmp(cmd, "how are you")) 
        {
            printf("I am fine, thanks.\n");
        } 
        //shows the current time when users enters "show the time"
        else if (!strcmp(cmd, "show the time")) 
        {
            int currentTimeInSeconds = getCurrentTimeInSeconds();
            int currentTimeInNanoseconds = getCurrentTimeInNanoseconds();
            if(currentTimeInSeconds != -1 && currentTimeInNanoseconds != -1)
            {
                printf("The time is %d seconds and %d nanoseconds since midnight, Jan 1, 1970.\n", currentTimeInSeconds, currentTimeInNanoseconds);
            }
            else
            {
                printf("Sorry, something went wrong while running show the time command.");
            }
        } 
        //provides static name when user enters "show your name"
        else if (!strcmp(cmd, "show your name")) 
        {
            printf("My name is Frank.\n");
        } 
        //prints the process id 
        else if (!strcmp(cmd, "show your id")) 
        {
            printf("My id is process %i.\n", getpid());
        } 
        //prints parent's process id
        else if (!strcmp(cmd, "show your parent's id")) 
        {
            printf("My parent's id is process %i.\n", getppid());
        } 
        //prints the name of the user when user enters "who am i"
        else if (!strcmp(cmd, "who am i")) 
        {
            if(getenv("USER") != NULL)
            {
                printf("You are %s, silly.\n", getenv("USER"));
            }
            else
            {
                printf("Sorry, something went wrong while getting USER. One possibility is that USER is null.\n");
            }
        }
        //calculates how long program is running by subtracting current time from the start time
        else if(!strcmp(cmd, "show your age"))
        {
            int currentTimeInSeconds = getCurrentTimeInSeconds();
            if(currentTimeInSeconds != -1)
            {
                int elapsedTime =  currentTimeInSeconds - startTimeInSeconds;
                printf("I am about %d seconds old.\n", elapsedTime);
            }
            else
            {
                printf("Error encountered while calculating the age");
            }
        }
        //prints the current directory when user enters "where am i"
        else if(!strcmp(cmd, "where am i"))
        {
            char cwd[PATH_MAX];
            //gets the current working directory
            if (getcwd(cwd, sizeof(cwd)) != NULL) 
            {
                printf("You are in directory: %s\n", cwd);
            } 
            else 
            {
                printf("Sorry, something went wrong while executing a function to get current directory");
            }
        }
        //given a directory, print a list of all the entries in that directory.
        else if(!strncmp(cmd, "list ", 5))
        {
            char* directory = splitString(cmd, ' ');
            // opendir() returns a pointer of DIR type. 
            DIR *openDir = opendir(directory);
            struct dirent *readDir;  // Pointer for directory entry
            //open the directory
            if (openDir == NULL)  // opendir returns NULL if couldn't open directory
            {
                printf("Could not open the entered directory\n" );
            }
            else
            {
                //read the directory
                printf("Ok, here are the things in directory %s:\n", directory);
                while ((readDir = readdir(openDir)) != NULL)
                {
                    printf("%s\n", readDir->d_name);
                }
                //closes the directory
                if(closedir(openDir) == -1)
                {
                    printf("Sorry, something went wrong while closing the directory");
                }    
            }          
        }
        //changes into the given directory.
        else if(!strncmp(cmd, "go to ", 6))
        {
            //splits by space and changes the directory
            if (chdir (splitString(splitString(cmd, ' '), ' ')) == -1)
            {
                printf("The directory could not be changed.\n");
            }
            else
            { 
                //prints the current directory which is changed
                char cwd[PATH_MAX];
                if (getcwd(cwd, sizeof(cwd)) != NULL) 
                {
                    printf("Ok, you are now in directory %s\n", cwd);
                } 
                else 
                {
                    printf("Sorry, something went wrong while executing a function to get current directory");
                }
            }
        }
        //open that file and print a message indicating the number for the 
        //corresponding file descriptor (or an error message if the file can't be opened)
        else if(!strncmp(cmd, "open ", 5))
        {
            char *fileName = splitString(cmd, ' '); 
            //open returns the descriptor
            int descriptor = open(fileName, O_RDONLY);
            if(descriptor == -1)
            {
                printf("Oops, that file could not be opened. Maybe input a different file name?\n");
            }
            else
            {
                printf("The number for %s file descriptor is %i.\n", fileName, descriptor);
            }
        }
        //read and print one line of text from the given file descriptor, or print an error if something went wrong
        //the error is printed from readline function
        else if(!strncmp(cmd, "read ", 5))
        {
                int read_descriptor = atoi(splitString(cmd, ' '));  // ascii-to-floatingpoint
                char* singleLine = readline(read_descriptor);
                printf("%s\n", singleLine);
                free(singleLine); // to free the malloc from readline
        }
        //close the given file descriptor, or print an error if something went wrong
        else if(!strncmp(cmd, "close ", 6))
        {
            int closing_descriptor = atoi(splitString(cmd, ' '));  // ascii-to-floatingpoint
            //prints message if user tries to close file 0, 1, or 2
            if(closing_descriptor <= 2)
            {
                printf("The closing descriptor should be greater than 2 as first few files should not be closed.\n");
            }
            else
            {
                 if(close(closing_descriptor) == -1)
                {
                    printf("Something went on wrong while closing the file with descriptor %i.\n", closing_descriptor);
                }
                else
                {
                    printf("File with given descriptor %i closed successfully.\n", closing_descriptor);
                }
            }
        }
        //prints the status of all files
        else if(!strcmp(cmd, "show file status"))
        {
            fstatus();
        }
        //exits the program
        else if (!strcmp(cmd, "quit")) 
        {
            printf("Bye bye now!\n");
            quit = 1;
        } 
        else 
        {
            printf("Sorry, I don't know how to do that.\n");
        }    
        // Free the command string, so we don't leak memory.
        free(cmd);
    }//end of while
}//end of main