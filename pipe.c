#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

int main(int argc, char *argv[])
{
	// Make the program work with a single command line argument (./pipe ls should output the files in directory)
	if(argc < 2){
		errno = EINVAL;
		perror("Must provide at least one process to run");
		exit(EINVAL);
	}
	// Try and run each process entered into the command line
	for (int i = 1; i < argc; i++){
		int pid = fork();
		wait(NULL);
		if (pid == 0) { // Child process
			// Have the child process call the first commmand line arg
			execlp(argv[1], argv[1], NULL);
		} else{
			continue;
		}
		i++;
	}
	return 0;
}
