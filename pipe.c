#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

int main(int argc, char *argv[])
{
	int processes[argc - 1];
	int i;
	// exit with EINVAL if no program args
	if (argc < 2) {
		errno = EINVAL;
		perror("Error occured");
		exit(errno);
	}
	
	for (i = 1; i < argc - 1; i++) {
		// creating the pipe 
		int fd[2];
		if (pipe(fd) < 0) {
			perror("Pipe");
			exit(errno);
		}
		int pid = fork(); 
		if (pid == 0) {
			if (dup2(fd[1], 1) < 0) {
				perror("Dup2");
				exit(errno);
			}
			close(fd[1]);
			close(fd[0]);
			if (execlp(argv[i], argv[i], NULL) < 0) {
				perror("execlp");
				exit(errno);
			}
		}
		else if (pid < 0) {
			perror("Fork");
			exit(errno);
		}
		// parent process
		processes[i] = pid;
		if (dup2(fd[0], 0) < 0) {
			perror("dup2");
			exit(errno);
		}
		close(fd[1]);
	}
	// for the last program in the list
	int last = fork();
	if (last < 0) {
		perror("Fork");
		exit(errno);
	}
	else if (last == 0) {
		execlp(argv[i], argv[i], NULL);
		perror("execlp");
		return(errno);
	}
	else {
		processes[i] = last;
	}
	for (int o = 0)

	int status = 0; // variable to hold the status of each children
	for (int i = 1; i < argc; i++) {
		waitpid(processes[i], &status, 0);
		if (WEXITSTATUS(status) != 0) {
			exit(WEXITSTATUS(status));
		}
	}
}
