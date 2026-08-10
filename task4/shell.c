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

// Function to check if background command
int isBackgroundCommand(char **tokens)
{
    int index = 0;

    for (index = 0; tokens[index] != NULL; index++)
    {
    };

    if (strcmp(tokens[index - 1], "&") == 0)
    {
        return index - 1;
    }

    return -1;
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

int execute(char *command, char *arguements[], int isBackground)
{
    int childProcessId = fork();
    int status;
    int exitCode = 0;

    if (isBackground != -1)
    {
        arguements[isBackground] = NULL; // remove trailing &
    }

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
        if (isBackground != -1)
        {
            // background process start
            printf("[SHELL|CHILD|BACKGROUND] Process with ID: %d started running in background \n", childProcessId);
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
    }

    return exitCode;
}

/* Redirection of Input Output */
int executeIfRedirection(char **tokens, int isBackground)
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
    if (isBackground != -1)
    {
        tokens[isBackground] = NULL; // remove trailing &
    }

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
        if (isBackground != -1)
        {
            // background process start
            printf("[SHELL|CHILD|BACKGROUND] Process with ID: %d started running in background \n", childProcessId);
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
    }

    return exitCode;
}

void executeSerializedCode(char *tokens[], int serialCharacterIndex)
{

    // make the && -> NULL
    tokens[serialCharacterIndex] = NULL;

    int isBackgroundOne = isBackgroundCommand(tokens);

    // try redirection
    int exitCodeOne = executeIfRedirection(tokens, isBackgroundOne);

    if (exitCodeOne == -1)
    {
        // no redirection
        exitCodeOne = execute(tokens[0], tokens, isBackgroundOne);
    }

    if (exitCodeOne == 0)
    {
        char **secondCommand = &tokens[serialCharacterIndex + 1];
        int isBackgroundTwo = isBackgroundCommand(secondCommand);

        // try redirection
        int exitCodeTwo = executeIfRedirection(secondCommand, isBackgroundTwo);

        if (exitCodeTwo == -1)
        {
            // execute normally no redirection
            exitCodeTwo = execute(secondCommand[0], secondCommand, isBackgroundTwo);
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
    int status, childPid;

    while (1)
    {

        // Background process cleaning
        while ((childPid = waitpid(-1, &status, WNOHANG)) > 0)
        {
            if (WIFEXITED(status))
            {
                int exitCode = WEXITSTATUS(status);
                printf("[BACKGROUND] Process %d done (exit status: %d) \n", childPid, exitCode);
            }
        }

        // Code starts here actual logic
        printf("shell> ");

        if (fgets(line, sizeof(line), stdin) == NULL)
            break;

        tokens = tokenize(line);

        if (tokens[0] == NULL)
        {
            /* Ignore empty input */
            /* Free allocated memory */
            for (i = 0; tokens[i] != NULL; i++)
                free(tokens[i]);

            free(tokens);
            continue;
        }

        if (strcmp(tokens[0], "exit") == 0)
        {
            /*exit command */
            /* Free allocated memory */
            for (i = 0; tokens[i] != NULL; i++)
                free(tokens[i]);

            free(tokens);
            break;
        }

        /*
         * 1. Create a child process using fork().
         * 2. In the child process, execute the command using execvp().
         * 3. If execvp() fails, print an error message and terminate the child.
         * 4. In the parent process, wait for the child to complete.
         */

        int isBackground = isBackgroundCommand(tokens);

        int serialized = containsSerializedCommand(tokens);

        if (serialized != -1)
        {
            executeSerializedCode(tokens, serialized);
            /* Free allocated memory */
            for (i = 0; tokens[i] != NULL; i++)
                free(tokens[i]);

            free(tokens);
            continue;
        }

        int executionStatus = executeIfRedirection(tokens, isBackground);
        if (executionStatus !=-1)
        {
            /* Free allocated memory */
            for (i = 0; tokens[i] != NULL; i++)
                free(tokens[i]);

            free(tokens);

            continue;
        }

        // execute
        execute(tokens[0], tokens, isBackground);

        /* Free allocated memory */
        for (i = 0; tokens[i] != NULL; i++)
            free(tokens[i]);

        free(tokens);
    }

    return 0;
}
