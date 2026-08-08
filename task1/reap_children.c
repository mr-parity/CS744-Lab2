#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>

#define NUM_CHILDREN 100

int main()
{
	pid_t pid;
	int status;
	int reaped = 0;
	int childPid = 0; // added to fix bug regarding incorrect pid prints of reaped child process

	printf("I am the Parent process.PID = %d\n", getpid());

	/* Create 100 child processes */
	for (int i = 0; i < NUM_CHILDREN; i++) {
		pid = fork();

		if (pid < 0) {
			perror("fork");
			exit(EXIT_FAILURE);
		}

		if (pid==0) {

			/* Child process */
            
			srand(time(NULL) ^ getpid());
			int sleep_time = rand() % 40 + 5; // each child sleeps for 5–45 seconds

			printf("I am Child %d with PID = %d\n", i + 1,
			       getpid());
			printf("Sleeping for %d seconds...\n\n", sleep_time);
			fflush(stdout);

			sleep(sleep_time);

			exit((i + 1) % 256);
		}
	}

	printf("Parent created %d child processes.\n\n", NUM_CHILDREN);

	/* Reap children using waitpid() with WNOHANG */
	while (reaped<NUM_CHILDREN) 
    {
		int found_child = 0;

		// WNOHANG is Wait No Hang non blocking wait for parent
		// >0 : to fix any encountered error
		while ((childPid = waitpid(-1,&status,WNOHANG))>0) {
			found_child = 1;
			reaped++;

			if (WIFEXITED(status)) {
				printf("[Parent] Reaped child PID = %d, Exit Status = %d "
				       "(Total Reaped = %d/%d)\n",
				       childPid, WEXITSTATUS(status), reaped,
				       NUM_CHILDREN);
			}
		}

		if (!found_child) {
			printf("[Parent] No child has terminated. Sleeping for 5 seconds...\n");
			sleep(5);
		}
	}

	printf("\nAll %d child processes have been reaped.\n", NUM_CHILDREN);
	return 0;
}


/*

========== Lab Questions ==========

1. What does waitpid(-1, &status, WNOHANG) return in each of the following cases?
Answer : The waitpid returns the process id of the terminated process/child process

	a.A child process has terminated.
	Answer: childs process id

	b. No child process has terminated.
	Answer: doesnt waits as the parent wait is non blocking and it returns 0 immediately

	c. There are no remaining child processes.
	Answer: returns -1

2. What would happen if the parent process never called wait() or waitpid()?
Answer: If the parent process never called wait or waitpid it would create the child process and
		be in continuous while loop or may exit and makes childs zombie process

3. Why is the first argument to waitpid() set to -1?
Answer: It mains returns pid of child process not waiting for any particular child but any child

4. What status does the parent process receive when a child exits with a status > 255, and
why?
Answer: The parent process receives a status value that has wrapped around via a bitwise AND 
		operation (status & 0xFF), effectively taking the value modulo 256.

		Unix/Linux systems allocate only 8 bits (1 byte) to store the exit status code passed from the child to the parent
		@cited source: https://man7.org/linux/man-pages/man3/exit.3.html
		
*/