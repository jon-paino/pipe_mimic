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
	// Start with stdin being 0 for the first process
	int fd_in = 0;
	int fd_out;
	// Put the output of each process into a tmp file for the input of the next
	for (int i = 1; i < argc; i++){
		char temp_file[] = "/tmp/tempfileXXXXXX";
		if(i < argc - 1){ // For every process but the last
			// Create a temp file and assign the output fd to this file
			fd_out = mkstemp(temp_file);
			if (fd_out == -1){
				perror("Failed to make temp file");
				return EXIT_FAILURE;
			}
			
		}
		// Create a child process and point its stdin to the previously created temp file
		if(fork() == 0){
			if(fd_in != 0){
				dup2(fd_in,0);
				close(fd_in); // Close fd_in as 0 is now an alias for it
			}
			if(i < argc - 1){
				dup2(fd_out, 1); // Set up stdout for this process to be the temp file
				close(fd_out); // Close fd_out as 1 is now an alias for it
			} else {
				// If it's the last command, we might want it to output to stdout
				if (fd_in != 0) {
					close(fd_in);
				}
			}
			execlp(argv[i],argv[i], NULL); // Run child process as current argument
			perror("execlp");
            exit(EXIT_FAILURE);
		} else {  // Parent process clean-up/set-up
            if (fd_in != 0) {
                close(fd_in);
            }
            if (i < argc - 1) {
                fd_in = open(temp_file, O_RDONLY);  // Next command reads from this temp file
                close(fd_out); 
				wait(NULL);
            }
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
