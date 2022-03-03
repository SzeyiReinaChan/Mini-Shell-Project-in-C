#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#define MAX_BUFFER_SIZE 80

//Creating Builtins
int buildin_cd(char **args);
int buildin_help(char **args);
int buildin_exit(char **args);
int buildin_history(char **args);

// array that store all the name of the buildin functions
char *builtin_str[] = {
    "cd",
    "help",
    "exit",
    "history"};

int (*builtin_func[])(char **) = {
    &buildin_cd,
    &buildin_help,
    &buildin_exit,
    &buildin_history};

int num_builtins()
{
    return sizeof(builtin_str) / sizeof(char *);
}

void sigint_handler(int sig)
{
    write(1, "  Mini Shell Terminated\n", 24);
    exit(0);
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
            perror("Error");
        }
        chdir(args[1]);
    }
    return 1;
}

int buildin_help(char **args)
{
    int i;
    printf("Below are the build in functions:\n");
    for (i = 0; i < num_builtins(); i++)
    {
        printf("  %s\n", builtin_str[i]);
    }

    return 1;
}

int buildin_exit(char **args)
{
    exit(1);
}

char **history_list;

int buildin_history(char **args)
{
    int i;
    printf("Printing Recent 10 Histories: \n");
    for (i = 0; i < 10; i++)
    {
        if (history_list[i] != NULL)
        {
            printf("%s", history_list[i]);
        }
    }
    return 1;
}

int non_buildin(char **myargv1)
{

    pid_t pid1;
    pid1 = fork();

    if (pid1 < 0)
    {
        printf("fork 1 failed for some reason!");
        return 1;
    }

    if (pid1 == 0)
    {
        execvp(myargv1[0], myargv1);
        printf("Command not found--Did you mean something else?\n");
    }
    else
    {
        wait(NULL);
    }
    return 1;
}

void non_buildin_pipe(char **myargv1, char **myargv2)
{
    int fd1[2];
    pipe(fd1);
    pid_t pid1;
    pid_t pid2;

    pid1 = fork();

    if (pid1 < 0)
    {
        printf("fork 1 failed for some reason!");
        exit(EXIT_FAILURE);
    }
    else if (pid2 < 0)
    {
        printf("fork 2 failed for some reason!");
        exit(EXIT_FAILURE);
    }

    if (pid1 == 0)
    {
        close(STDOUT_FILENO);
        dup2(fd1[1], STDOUT_FILENO);
        close(fd1[1]);
        close(fd1[0]);
        execvp(myargv1[0], myargv1);
        printf("Command not found--Did you mean something else?\n");
    }
    else
    {
        wait(NULL);
        pid2 = fork();

        if (pid2 == 0)
        {
            close(STDIN_FILENO);
            dup2(fd1[0], STDIN_FILENO);
            close(fd1[1]);
            close(fd1[0]);
            execvp(myargv2[0], myargv2);
            printf("Command not found--Did you mean something else?\n");
        }
        else
        {
            wait(NULL);
            close(fd1[1]);
            close(fd1[0]);
        }
    }
}

void test_multi_commands(char **tokens, int bar_pos)
{
    int x = 0;
    int y = 0;
    char **front = malloc(MAX_BUFFER_SIZE * sizeof(char *));
    char **back = malloc(MAX_BUFFER_SIZE * sizeof(char *));

    //getting the front part of the command
    for (x = 0; x < bar_pos; x++)
    {
        front[x] = tokens[x];
    }
    front[x] = NULL;

    //getting the back part of the command
    for (x = bar_pos + 1, y = 0; tokens[x]; x++, y++)
    {
        back[y] = tokens[x];
    }
    back[y] = NULL;

    non_buildin_pipe(front, back);

    free(front);
    free(back);
}

//Main
int main()
{
    alarm(60);

    //READLINE START HERE:
    char line[MAX_BUFFER_SIZE]; // A buffer to hold 80 characters at most
    int history_size = 0;
    history_list = malloc(10 * sizeof(char *));

    // A loop that runs forever.
    while (1)
    {
        signal(SIGINT, sigint_handler);

        int i = 0;
        int bar_pos = 0;
        int flag = 0;
        int exe = 0;

        printf("mini-shell> ");

        // Reading input
        fgets(line, MAX_BUFFER_SIZE, stdin);

        //create array to save tokens
        char **tokens = malloc(MAX_BUFFER_SIZE * sizeof(char *));
        char *token;

        if (strcmp(line, "") != 0)
        {
            history_list[history_size % 10] = strdup(line);
        }
        history_size++;

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
        tokens[i] = NULL;

        //check if input contain |
        for (bar_pos = 0; tokens[bar_pos]; bar_pos++)
        {
            if (strcmp(tokens[bar_pos], "|") == 0)
            {
                exe = 1;
                break;
            }
        }

        //check if user input match any buildin command
        if (exe == 0)
        {
            printf("Hey!");

            for (i = 0; i < num_builtins(); i++)
            {
                if (strcmp(tokens[0], builtin_str[i]) == 0)
                {
                    (builtin_func[i])(tokens);
                    flag = 1;
                    break;
                }
            }

            //check what function to use

            if (flag != 1)
            {
                non_buildin(tokens);
            }

            free(tokens);
        }
        test_multi_commands(tokens, bar_pos);
    }

    int index = 0;
    for (index = 0; index < 10; index++)
    {
        free(history_list[index]);
    }
    free(history_list);
    return 0;
}
