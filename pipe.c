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
	execlp(argv[1], argv[1], NULL);
	return 0;
}
