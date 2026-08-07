#include <iostream>
#include <unistd.h>

// function that accepts a pointer to function which taked void as a parameter and returns void
int createChild(void (*task)(int &childNumber), int childNumber, int sleepTime);

void childTask(int &childNumber);
void parentTask(int &, int &);
void childExitMessage(int, int &);

int main()
{

    // create first child
    int firstChildProcessId = createChild(&childTask, 1, 5);
    int secondChildProcessId = 0;
    int statusCode;

    // create second child with safety
    if (firstChildProcessId > 0)
    {
        secondChildProcessId = createChild(&childTask, 2, 2);
    }

    // parents code
    parentTask(firstChildProcessId, secondChildProcessId);

    // wait for childProcess
    waitpid(firstChildProcessId, &statusCode, 0);
    childExitMessage(1, statusCode);

    // wait for second Child (already exited but codes reside in page table and process as zombie)
    waitpid(secondChildProcessId, &statusCode, 0);
    childExitMessage(2, statusCode);

    return 0;
}

int createChild(void (*childTask)(int &childNumber), int childNumber, int sleepTime)
{
    int childProcessId = fork();

    // handle failure
    if (childProcessId < 0)
    {
        std::cout << "Failed to create child process!" << std::endl;
        return -1;
    }
    else if (childProcessId == 0)
    {
        // call child Task
        childTask(childNumber);

        // sleep for some time
        sleep(sleepTime);

        // exit
        _exit(0);
    }

    return childProcessId;
}

void childTask(int &childNumber)
{
    // process details
    std::cout << "---------- Child Process Details ----------" << std::endl;
    std::cout << "Child Number: " << childNumber << std::endl;
    std::cout << "Child process ID: " << getpid() << std::endl;
    std::cout << "Child's parent process ID: " << getppid() << std::endl;
}

void parentTask(int &firstChildId, int &secondChildId)
{
    // process details
    std::cout << "---------- Ordered Wait Process Details ----------" << std::endl;
    std::cout << "Ordered Wait process ID: " << getpid() << std::endl;
    std::cout << "First child process ID:  " << firstChildId << std::endl;
    std::cout << "Second child process ID:  " << secondChildId << std::endl;
}

void childExitMessage(int childNumber, int &statusCode)
{
    // if child exited normally
    if (WIFEXITED(statusCode))
    {
        int exitCode = WEXITSTATUS(statusCode);
        std::cout<<"---------- Termination Details ----------"<<std::endl;
        std::cout << "Child process " << childNumber << " terminated!" << std::endl;
        std::cout << "Child " << childNumber << " Exit Code: " << exitCode << std::endl;
    }
}