#include <iostream>
#include <unistd.h>

int main()
{
    // duplicate the file descriptors of std i/o
    int copyOfStdInput = dup(0);
    int copyOfStdOutput = dup(1);

    // continue to read and write till EOF
    char tempBufferStorage[100];

    while(true)
    {
        // read from std input
        int bytedRead = read(copyOfStdInput,tempBufferStorage,sizeof(tempBufferStorage));
        
        // no data read or EOF
        if(bytedRead<=0){
            break;
        }

        // write to std output
        write(copyOfStdOutput,tempBufferStorage,bytedRead);
    }

    // close file descriptors
    close(copyOfStdInput);
    close(copyOfStdOutput);

    return 0;
}