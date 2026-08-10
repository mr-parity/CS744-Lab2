#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>

#define MAX_INPUT_SIZE 1024
#define MAX_TOKEN_SIZE 64
#define MAX_NUM_TOKENS 64

/* Splits the input string into space-separated tokens */
char **tokenize(char *line)
{
    char **tokens = (char **)malloc(MAX_NUM_TOKENS * sizeof(char *));
    char *token = (char *)malloc(MAX_TOKEN_SIZE * sizeof(char));

    int i, tokenIndex = 0, tokenNo = 0;

    for (i = 0; i < strlen(line); i++)
    {
        char ch = line[i];

        if (ch == ' ' || ch == '\t' || ch == '\n')
        {
            token[tokenIndex] = '\0';

            if (tokenIndex != 0)
            {
                tokens[tokenNo] = (char *)malloc(MAX_TOKEN_SIZE);
                strcpy(tokens[tokenNo], token);
                tokenNo++;
                tokenIndex = 0;
            }
        }
        else
        {
            token[tokenIndex++] = ch;
        }
    }

    free(token);
    tokens[tokenNo] = NULL;

    return tokens;
}

/* Fuunction to check for serialized commands returns -1 if false else index of first &*/

int containsSerializedCommand(char *tokens[])
{
    for (int i = 0; tokens[i] != NULL; i++)
    {
        if (strcmp(tokens[i], "&&") == 0)
        {
            return i;
        }
    }

    return -1;
}

int execute(char *command, char *arguements[])
{
    int childProcessId = fork();
    int status;
    int exitCode = 0;

    if (childProcessId < 0)
    {
        perror("[PARENT] Failed to create child process");
    }
    else if (childProcessId == 0)
    {
        execvp(command, arguements);

        // failure
        perror("[SHELL|CHILD]: Execution Failed!");
        _exit(127);
    }
    else
    {
        waitpid(childProcessId, &status, 0);

        if (WIFEXITED(status))
        {
            exitCode = WEXITSTATUS(status);
            printf("[Exit Code: %d ]\n", exitCode);
        }
    }

    return exitCode;
}

/* Redirection of Input Output */
int executeIfRedirection(char **tokens)
{
    // check for < or >
    int redirectedIndex = -1;
    char *operator;
    int status = 10, exitCode = 10;

    for (int i = 0; tokens[i] != NULL; i++)
    {
        if (strcmp(tokens[i], "<") == 0 || strcmp(tokens[i], ">") == 0)
        {
            redirectedIndex = i;
            operator = tokens[i];
            break;
        }
    }

    // stop if no redirection
    if (redirectedIndex == -1)
        return -1;

    // execute redirection

    int childProcessId = fork();

    if (childProcessId < 0)
    {
        perror("Failed to spawn child process!");
    }
    else if (childProcessId == 0)
    {
        if (strcmp(operator, "<") == 0)
        {
            // input redirect
            int fileToRead = open(tokens[redirectedIndex + 1], O_RDONLY);

            if (fileToRead < 0)
            {
                perror("Failed to open file for reading!");
                _exit(127);
            }

            dup2(fileToRead, 0);

            tokens[redirectedIndex] = NULL; // exclude from opr to execute

            execvp(tokens[0], tokens);

            // failure
            perror("[SHELL|CHILD] Execution Failed");
            close(fileToRead);
            _exit(127);
        }
        else
        {
            // output redirect
            int fileToWrite = open(tokens[redirectedIndex + 1], O_CREAT | O_WRONLY | O_TRUNC, 0644);

            if (fileToWrite < 0)
            {
                perror("Failed to open file for writing!");
                _exit(127);
            }

            dup2(fileToWrite, 1);

            tokens[redirectedIndex] = NULL; // exclude from opr to execute

            execvp(tokens[0], tokens);

            // failure
            perror("[SHELL|CHILD] Execution Failed");
            close(fileToWrite);
            _exit(127);
        }
    }
    else
    {
        waitpid(childProcessId, &status, 0);

        if (WIFEXITED(status))
        {
            exitCode = WEXITSTATUS(status);
            printf("[Exit Code: %d ] \n", exitCode);
        }
    }

    return exitCode;
}

void executeSerializedCode(char *tokens[], int serialCharacterIndex)
{

    // make the && -> NULL
    tokens[serialCharacterIndex] = NULL;

    // try redirection
    int exitCodeOne = executeIfRedirection(tokens);

    if (exitCodeOne == -1)
    {
        // no redirection
        exitCodeOne = execute(tokens[0], tokens);
    }

    if (exitCodeOne == 0)
    {
        char **secondCommand = &tokens[serialCharacterIndex + 1];

        // try redirection
        int exitCodeTwo = executeIfRedirection(secondCommand);

        if (exitCodeTwo == -1)
        {
            // execute normally no redirection
            exitCodeTwo = execute(secondCommand[0], secondCommand);
        }

        if (exitCodeTwo != 0)
        {
            printf("Failed to run process two! \n");
        }
    }
}

int main()
{
    char line[MAX_INPUT_SIZE];
    char **tokens;
    int i;
    int status;

    while (1)
    {
        printf("shell> ");

        if (fgets(line, sizeof(line), stdin) == NULL)
            break;

        tokens = tokenize(line);

        if (tokens[0] == NULL)
        {
            /* Ignore empty input */
            continue;
        }

        if (strcmp(tokens[0], "exit") == 0)
        {
            /*exit command */
            break;
        }

        /*
         * 1. Create a child process using fork().
         * 2. In the child process, execute the command using execvp().
         * 3. If execvp() fails, print an error message and terminate the child.
         * 4. In the parent process, wait for the child to complete.
         */

        int serialized = containsSerializedCommand(tokens);

        if (serialized != -1)
        {
            executeSerializedCode(tokens, serialized);
            continue;
        }

        int executionStatus = executeIfRedirection(tokens);
        if (executionStatus == 0)
        {
            continue;
        }
        
        // execute
        execute(tokens[0], tokens);

        /* Free allocated memory */
        for (i = 0; tokens[i] != NULL; i++)
            free(tokens[i]);

        free(tokens);
    }

    return 0;
}
