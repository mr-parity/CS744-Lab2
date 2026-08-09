#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

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

int execute(char* command,char* arguements[])
{
    int childProcessId = fork();
    int status;
    int exitCode = 0;

    if(childProcessId<0)
    {
        perror("[PARENT] Failed to create child process");
    }
    else if (childProcessId==0)
    {
        execvp(command,arguements);

        // failure
        perror("[SHELL|CHILD]: Execution Failed!");
        _exit(127);
    }
    else 
    {
        waitpid(childProcessId,&status,0);

        if(WIFEXITED(status))
        {
            exitCode = WEXITSTATUS(status);
        }
    }

    return exitCode;
}

void executeSerializedCode(char* tokens[],int serialCharacterIndex)
{

    // amke the && -> NULL
    tokens[serialCharacterIndex] = NULL;

    int exitCodeOne = execute(tokens[0],tokens);

    if(exitCodeOne==0)
    {
        int exitCodeTwo = execute(tokens[serialCharacterIndex+1],&tokens[serialCharacterIndex+1]);

        if(exitCodeTwo!=0)
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

        if(serialized!=-1)
        {
            executeSerializedCode(tokens,serialized);
            continue;
        }

        int childProcessId = fork();

        if(childProcessId<0)
        {
            perror("Failed to create child process! \n");
        }
        else if(childProcessId==0)
        {
            // child process
            execvp(tokens[0],tokens);
            
            printf("[SHELL|CHILD] Execeution Failed... \n");
            _exit(0); // terminate the child
        }
        else
        {
            // parent process
            waitpid(childProcessId,NULL,0);

        }

        /* Free allocated memory */
        for (i = 0; tokens[i] != NULL; i++)
            free(tokens[i]);

        free(tokens);
    }

    return 0;
}


