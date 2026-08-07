#include <iostream>
#include <unistd.h>

int main()
{
    // create child process using fork
    int childProcess = fork();

    if (childProcess == -1)
    {
        std::cerr << "Failed to create child process!" << std::endl;
    }
    else if (childProcess == 0)
    {
        // child process
        std::cout << "---------- Child Process Details ----------" << std::endl;
        std::cout << "Child process ID: " << getpid() << std::endl;
        std::cout << "Child's parent process ID: " << getppid() << std::endl;
        
        // random wait
        sleep(2);

        //exit
        _exit(0);
    }
    else
    {
        // parent process
        std::cout << "---------- Reap Child Process Details ----------" << std::endl;
        std::cout << "Reap Child process ID: " << getpid() << std::endl;
        std::cout << "Child process ID: " << childProcess << std::endl;

        // wait for child to finish
        waitpid(childProcess, nullptr, 0);

        std::cout << "Child process " << childProcess << "has finished!" << std::endl;
    }
}