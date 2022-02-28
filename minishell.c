// gcc readline.c -o readline
//
// This example program demonstrates
// how to read a line in C and
// use the string compare function.
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#define MAX_BUFFER_SIZE 80

void sigint_handler(int sig)
{
    write(1, "Mini Shell Terminated\n", 22);
    exit(0);
}

int main()
{
    alarm(60);

    //READLINE START HERE:
    char line[MAX_BUFFER_SIZE]; // A buffer to hold 80 characters at most
    signal(SIGINT, sigint_handler);
    // A loop that runs forever.
    while (1)
    {
        printf("mini-shell>");
        // Read in 1 line of text
        // The line is coming from 'stdin' standard input
        fgets(line, MAX_BUFFER_SIZE, stdin);
        // We write out to standard output
        printf("Here is what you typed: %s\n", line);

        // Let's see what happens when comparing a string.
        // Think about this behavior.
        if (strcmp(line, "help") == 0)
        {
            printf("You typed in help\n");
        }
        if (strcmp(line, "help\n") == 0)
        {
            printf("You typed in help with an endline\n");
        }
        sleep(1);
    }

    return 0;
}
