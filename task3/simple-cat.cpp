#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

// function declaration
void readAndWriteConsole();
void readFileWriteConsole(char**);

int main(int argc, char *argv[])
{

    int childProcessId = fork();

    if (childProcessId < 0)
    {
        std::cerr << "[PARENT] Failed to create child Process" << std::endl;
    }
    else if (childProcessId == 0)
    {

        if (argc < 2)
        {
            // read from console
            readAndWriteConsole();
        }
        else
        {
            readFileWriteConsole(argv);
        }

        std::cout<<"[CHILD] Process finished. Exiting..."<<std::endl;
        _exit(0);
    }
    else
    {
        waitpid(childProcessId, nullptr, 0); // blocking wait
        std::cout<<"[Parent] Exiting..."<<std::endl;
    }

    return 0;
}

// function definition
void readAndWriteConsole()
{
    int copyStdInput = dup(0);
    int copyStdOutput = dup(1);
    char inputBuffer[1024];

    // read and write till EOF
    while (true)
    {
        int bytesRead = read(copyStdInput, inputBuffer, sizeof(inputBuffer));

        if (bytesRead <= 0)
        {
            break;
        }

        write(copyStdOutput, inputBuffer, bytesRead);
    }

    // close file descriptors
    close(copyStdInput);
    close(copyStdOutput);
    
}

void readFileWriteConsole(char** argv)
{
    char buffer[1024];

    // read file
    int fileToRead = open(argv[1],O_RDONLY);

    if(fileToRead<0)
    {
        perror("Failed to read from file"); // print error message with details of failure
        return;
    }

    int bytesRead = 0;

    // read and write till bytes are left to be read
    while((bytesRead = read(fileToRead,buffer,sizeof(buffer)))>0)
    {
        write(1,buffer,bytesRead);
    }

    // close the file descriptor
    close(fileToRead);
    
}