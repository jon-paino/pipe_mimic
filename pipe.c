#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        errno = EINVAL;
        fprintf(stderr, "Usage: %s command1 [command2 ...]\n", argv[0]);
        exit(EINVAL);
    }

    int pipe_fd[2];
    int prev_fd_in = 0;  // Start with stdin being 0 for the first process

    for (int i = 1; i < argc; i++) {
        if (i < argc - 1) {
            if (pipe(pipe_fd) == -1) {
                perror("pipe");
                exit(EXIT_FAILURE);
            }
        }

        pid_t pid = fork();
        if (pid == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        }

        if (pid == 0) {  // Child process
            if (prev_fd_in != 0) {
                dup2(prev_fd_in, 0);  // Use previous input for stdin
                close(prev_fd_in);   // Close original fd
            }

            if (i < argc - 1) {
                dup2(pipe_fd[1], 1);  // Direct stdout to the write-end of the pipe
                close(pipe_fd[0]);   // Close read-end in the child
                close(pipe_fd[1]);   // Close original fd
            }

            execlp(argv[i], argv[i], NULL);
            perror("execlp");
            exit(EXIT_FAILURE);
        }

        // Parent process
        if (prev_fd_in != 0) {
            close(prev_fd_in);  // Close previous input
        }

        if (i < argc - 1) {
            prev_fd_in = pipe_fd[0];  // Store the read-end for the next iteration
            close(pipe_fd[1]);       // Close write-end in the parent
        }

		int status;
        wait(&status);  // Wait for the child process to finish
		if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
            exit(WEXITSTATUS(status));  // Exit with non-zero status if any child failed
        }
    }

    return 0;
}