#include <iostream>
#include <unistd.h>

/*
Function declarations
*/

void childTask(int &, char **);
int createChildProcess(int, char **, std::function<void(int &, char **)>);

int main(int argc, char *argv[])
{

    // safety check no commands
    if (argc < 2)
    {
        std::cerr << "No command entered. Type commands alongside executable [./executable command args]" << std::endl;
        return 0;
    }

    // create child process using fork
    int childProcessId = createChildProcess(argc, argv, childTask);

    // wait for child process
    if (childProcessId > 0)
    {
        int status;

        // print details of process
        std::cout << "---------- CMD EXEC Process Details ----------" << std::endl;
        std::cout << "[PARENT] Parent Process ID: " << getpid() << std::endl;
        std::cout << "[PARENT] Child Process ID: " << childProcessId << std::endl;

        // wait for child process till it completes execution
        waitpid(-1, &status, 0);

        // if process exited normally
        if (WIFEXITED(status))
        {
            int exitCode = WEXITSTATUS(status);

            if (exitCode == 0)
            {
                std::cout << "[PARENT] Child process completed successfully." << std::endl;
                std::cout << "[PARENT] Child process exited with code: " << exitCode << std::endl;
            }
            else
            {
                std::cout << "[PARENT] Child process failed with error code: " << exitCode << std::endl;
            }
        }
    }

    return 0;
}

int createChildProcess(int argc, char **argv, std::function<void(int &, char **)> childTask)
{
    int processId = fork();

    if (processId < 0)
    {
        std::cout << "Failed to create child process!" << std::endl;
    }
    else if (processId == 0)
    {
        // execute child task
        childTask(argc, argv);
    }

    return processId;
}

void childTask(int &argc, char **argv)
{

    // get command
    const char *command = argv[1];

    // get arguements
    char **arguements = &argv[1];

    execvp(command, arguements);

    // in case of failure
    std::cerr << "Execution failed." << std::endl;
    std::cout << "Child process terminated." << std::endl;
    _exit(127);
}