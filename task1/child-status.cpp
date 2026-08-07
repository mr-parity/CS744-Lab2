#include <iostream>
#include <unistd.h>

int main()
{

    // create child process
    int childProcessId = fork();

    if (childProcessId == -1)
    {
        std::cerr << "Failed to create child process!" << std::endl;
    }
    else if (childProcessId == 0)
    {
        // child process
        std::cout << "---------- Child Process Details ----------" << std::endl;
        std::cout << "Child process ID: " << getpid() << std::endl;
        std::cout << "Child's parent process ID: " << getppid() << std::endl;

        // sleep for some time
        sleep(2);

        // exit with status code 10
        _exit(10);
    }
    else
    {
        // parent process
        std::cout << "---------- Child Status Process Details ----------" << std::endl;
        std::cout << "Child Status process ID: " << getpid() << std::endl;
        std::cout << "Child parent process ID: " << childProcessId << std::endl;

        // status code
        int statusCode;

        // wait for childProcess
        waitpid(childProcessId, &statusCode, 0);

        // wait if exited : child process exited normally
        if (WIFEXITED(statusCode))
        {
            int exitCode = WEXITSTATUS(statusCode);
            std::cout << "Child Process exited normally!" << std::endl;
            std::cout << "Child Process exit code: " << exitCode << std::endl;
        }
    }
}