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

//Creating Builtins
int buildin_cd(char **args);
int buildin_help(char **args);
int buildin_exit(char **args);
// int buildin_history(char **args);

// array that store all the name of the buildin functions
char *builtin_str[] = {
    "cd",
    "help",
    "exit"};
// "buildin_history"};

int (*builtin_func[])(char **) = {
    &buildin_cd,
    &buildin_help,
    &buildin_exit};
// &buildin_history};

int num_builtins()
{
    return sizeof(builtin_str) / sizeof(char *);
}

int buildin_cd(char **args)
{
    if (args[1] == NULL)
    {
        fprintf(stderr, "Command not found--Did you mean something else?\n");
    }
    else
    {
        if (chdir(args[1]) != 0)
        {
            perror("lsh");
        }
        chdir(args[1]);
        printf("You are now in : %s\n", args[1]);
    }
    return 1;
}

int buildin_help(char **args)
{
    int i;
    for (i = 0; i < num_builtins(); i++)
    {
        printf("  %s\n", builtin_str[i]);
    }

    // printf("in help\n");
    return 1;
}

int buildin_exit(char **args)
{
    exit(1);
}

int non_buildin()
{
    // pid_t pid1;
    // int x = 1;
    // pid1 = fork();

    // if (pid1 == 0)
    // {
    //     char *myargv1[2];
    //     myargv1[0] = "ls";
    //     myargv1[1] = NULL;
    //     execvp(myargv1[0], myargv1);
    // }
    // else
    // {
    //     printf("parent=%d\n", getpid());
    // }
}

//Main
int main()
{
    alarm(60);

    //READLINE START HERE:
    char line[MAX_BUFFER_SIZE]; // A buffer to hold 80 characters at most
    signal(SIGINT, sigint_handler);
    // A loop that runs forever.
    while (1)
    {
        int i = 0;
        printf("mini-shell> ");
        // Read in 1 line of text
        // The line is coming from 'stdin' standard input
        fgets(line, MAX_BUFFER_SIZE, stdin);
        // printf("Here is what you typed: %s\n", line);

        //create array to save tokens
        char **tokens = malloc(MAX_BUFFER_SIZE * sizeof(char *));
        char *token;

        //getting words one by one and create first token in tonkens
        token = strtok(line, " \n"); //get word
        tokens[i] = token;
        i++;

        while (token != NULL)
        {
            token = strtok(NULL, " \n");
            tokens[i] = token;
            i++;
        }

        //check if user input match any buildin command
        for (i = 0; i < num_builtins(); i++)
        {
            if (strcmp(tokens[0], builtin_str[i]) == 0)
            {
                (builtin_func[i])(tokens);
            }
        }

        free(tokens);
        sleep(1);
    }

    return 0;
}
