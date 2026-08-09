#include<iostream>
#include <unistd.h> // unix std header
#include <fcntl.h> // file control header

int main()
{

    // create or open a file for write
    /*
        O_APPEND alone doesnt allow writing a file
    */
    int outputFileDescriptor = open("dup2.txt",O_APPEND|O_CREAT|O_WRONLY,0644);

    if(outputFileDescriptor==-1)
    {
        std::cout<<"Error opening file..."<<std::endl;
    }

    // using dup2 to redirect output to file
    dup2(outputFileDescriptor,1);

    // print
    std::cout<<"Hello, World!"<<std::endl;

    // close file descriptor
    close(outputFileDescriptor);
    return 0;
}