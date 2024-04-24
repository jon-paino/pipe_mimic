#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

int main(int argc, char *argv[])
{
	if(argc < 2){
		errno = EINVAL;
		fprintf(stderr, "Usage: %s command1 [command2 ...]\n", argv[0]);
		exit(EINVAL);
	}
	// Create file descriptors for a new a previous pipe for interprocess communication
	int prev_fd[2];
	int new_fd[2];

	for(int i = 1; i < argc; i++){
		// If it is not the last command, create a pipe for the current processes output
		if (i < argc - 1){
			pipe(new_fd);
		}
		if(fork() == 0){ // Child process
			// If it is not the first command then read input from previous pipe
			if (argc > 1){
				dup2(prev_fd[0], STDIN_FILENO);
				close(prev_fd[0]);
			}
			// If it is not the last command then send output to the new pipe
			if (i < argc - 1){
				dup2(new_fd[1], STDOUT_FILENO);
				close(new_fd[1]);
			}
			// Execute the current process
			execlp(argv[i], argv[i], NULL);
			perror("execlp failed");
            exit(EXIT_FAILURE);
		} else {  // Parent process
			// Close the reading end of the previous pipe if its not the first command
			if (argc > 1){
				close(prev_fd[0]);
			}
			// Set the previous pipe to the new pipe if its not the last command
			if (i < argc - 1){
				prev_fd[0] = new_fd[0];
				prev_fd[1] = new_fd[1];
			}
			wait(NULL);
		}
	}
	int status;
	while ((wait(&status)) > 0) {
		if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
			exit(WEXITSTATUS(status)); // Exit with the error code of the first failing child
		}
	}

	return 0;
}