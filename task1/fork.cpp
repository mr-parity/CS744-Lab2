#include <iostream>
#include <unistd.h>

int main()
{
    int childProcess = fork();

    if(childProcess==-1) // fork failed
    {
        std::cout<<"Failed to create child process."<<std::endl;
    }
    else if(childProcess==0) // child process
    {
        std::cout<< "---------- Child Process Details ----------"<<std::endl;
        std::cout<< "Child process ID: "<<getpid()<<std::endl;
        std::cout<< "Child's parent process ID: "<< getppid()<<std::endl;
    }
    else // main/parent process
    {
        std::cout<< "---------- Main Process Details ----------"<<std::endl;
        std::cout<< "Fork process ID: "<<getpid()<<std::endl;
        std::cout<< "Fork's child process ID: "<< childProcess <<std::endl;
    }

    return 0;
}