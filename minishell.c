#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#define MAX_BUFFER_SIZE 80
#define HISTORY_MAX_SIZE 10

//Creating Builtins
int buildin_cd(char **args);
int buildin_help(char **args);
int buildin_exit(char **args);
int buildin_history(char **args, char **history_list);

// array that store all the name of the buildin functions
char *builtin_str[] = {
    "cd",
    "help",
    "exit"};

int (*builtin_func[])(char **) = {
    &buildin_cd,
    &buildin_help,
    &buildin_exit};

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

int buildin_history(char **args, char **history_list)
{
    int i;
    printf("Printing Recent 10 Histories: \n");
    for (i = 0; i < HISTORY_MAX_SIZE; i++)
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
        exit(1);
    }

    if (pid1 == 0)
    {
        execvp(myargv1[0], myargv1);
        printf("Command not found--Did you mean something else?\n");
        exit(1);
    }
    else
    {
        wait(NULL);
    }
    return 1;
}

int non_buildin_pipe(char **myargv1, char **myargv2)
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

    if (pid1 == 0)
    {
        close(STDOUT_FILENO);
        dup2(fd1[1], STDOUT_FILENO);
        close(fd1[1]);
        close(fd1[0]);
        execvp(myargv1[0], myargv1);
        printf("Command not found--Did you mean something else?\n");
        exit(1);
    }
    else
    {
        wait(NULL);
        pid2 = fork();

        if (pid2 < 0)
        {
            printf("fork 2 failed for some reason!");
            exit(EXIT_FAILURE);
        }

        if (pid2 == 0)
        {
            close(STDIN_FILENO);
            dup2(fd1[0], STDIN_FILENO);
            close(fd1[1]);
            close(fd1[0]);
            execvp(myargv2[0], myargv2);
            printf("Command not found--Did you mean something else?\n");
            exit(1);
        }
        else
        {
            close(fd1[1]);
            close(fd1[0]);
            wait(NULL);
        }
    }
    return 1;
}

int test_multi_commands(char **tokens, int bar_pos)
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

    return 1;
}

int check_buildin(char **tokens)
{
    int i = 0;
    int build_in = 0;
    for (i = 0; i < num_builtins(); i++)
    {
        if (strcmp(tokens[0], "history") == 0)
        {
            build_in = 1;
            break;
        }
        else if (strcmp(tokens[0], builtin_str[i]) == 0)
        {
            build_in = 2;
            break;
        }
    }

    switch (build_in)
    {
    case 0:
        non_buildin(tokens);
        break;
    case 1:
        (&buildin_history)(tokens, history_list);
        break;
    case 2:
        (builtin_func[i])(tokens);
        break;
    default:
        break;
    }
    return 1;
}

int check_bar(char **tokens)
{
    int bar_pos = 0;
    int multi_commands = 0;
    for (bar_pos = 0; tokens[bar_pos]; bar_pos++)
    {
        if (strcmp(tokens[bar_pos], "|") == 0)
        {
            multi_commands = 1;
            break;
        }
    }

    switch (multi_commands)
    {
    case 0:
        check_buildin(tokens);
        break;
    case 1:
        test_multi_commands(tokens, bar_pos);
        break;
    default:
        break;
    }

    return 1;
}

//Main
int main()
{
    alarm(180);
    signal(SIGINT, sigint_handler);
    //READLINE START HERE:
    char line[MAX_BUFFER_SIZE]; // A buffer to hold 80 characters at most
    int history_size = 0;
    history_list = malloc(HISTORY_MAX_SIZE * sizeof(char *));

    // A loop that runs forever.
    while (1)
    {
        int i = 0;

        printf("mini-shell> ");

        // Reading input
        fgets(line, MAX_BUFFER_SIZE, stdin);

        //create array to save tokens
        char **tokens = malloc(MAX_BUFFER_SIZE * sizeof(char *));
        char *token;

        //saving history
        if (strcmp(line, "") != 0)
        {
            history_list[history_size % HISTORY_MAX_SIZE] = strdup(line);
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

        check_bar(tokens);

        free(tokens);
    }

    int index;
    for (index = 0; index < HISTORY_MAX_SIZE; index++)
    {
        free(history_list[index]);
    }

    free(history_list);

    return 0;
}
