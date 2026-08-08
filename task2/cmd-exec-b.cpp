#include <iostream>
#include <unistd.h>

int main(int argc, char* argv[])
{

    std::cout<<"---------- CMD EXEC Process Details ----------"<<std::endl;
    std::cout<<"Process ID: "<<getpid()<<std::endl;

    // if no commands entered exit
    if(argc<2)
    {
        return 0;
    }

    // get command
    const char* command = argv[1];

    // get command + rest of args
    char** arguements = &argv[1];

    // exec the command
    execvp(command, arguements);

    std::cerr<<"Failed to execute command."<<std::endl;
    return 0;
}