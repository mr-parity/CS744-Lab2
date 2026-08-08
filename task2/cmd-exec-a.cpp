#include <iostream>
#include <unistd.h>

int main()
{
    std::cout<<"---------- CMD EXEC Process Details ----------"<<std::endl;
    std::cout<<"Process ID: "<<getpid()<<std::endl;

    // exec commmand

    const char* command = "ls";  // pointer to a const char (immutable string)
    const char* argv[]  = {"ls","-l", nullptr}; // argv is an array of pointers to (immutable strings)


    std::cout<<"Executing ls -l ..."<<std::endl;

    // cast to mutable strings to fix compiler warning (ls doesnt modify argv)
    execvp(command,const_cast<char* const*> (argv));

    std::cerr<<"Failed to execute command."<<std::endl;

    return 0;

}