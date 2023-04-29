#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#define EXIT_FAILURE 1

int main(int argc, char *argv[])
{
	printf("begin\n");

	int process_ids[argc];
	int fds[2];

	if (pipe(fds)) {
		exit(EXIT_FAILURE);
	}

	int rc = fork();
	if (rc < 0) {
		exit(EXIT_FAILURE);
	}

	else if (rc == 0) {
		printf("child process 1 (before loop)");
		dup2(fds[1], STDOUT_FILENO);
		// dup2(fds[0], STDIN_FILENO);
		int execlp_ret = execlp(argv[1], argv[1], (char *) NULL);
		perror("execlp");
		exit(EXIT_FAILURE);
	}

	else {
		int status = 0;
		waitpid(rc, &status, 0);
		close(fds[1]); // close the write end - close the read end only after we've read from it in the next process
		printf("Child process 1 exits with code: %d\n", WEXITSTATUS(status));
	}

	// int fd_read = fds[0];

	for (int i = 2; i < argc - 1; i++) {
		// close fds[0] here
		close(fds[0]);
		
		if (pipe(fds)) {
			exit(EXIT_FAILURE);
		}	

		int rc1 = fork(); // return code
		if (rc1 < 0) {
			exit(EXIT_FAILURE);
		}

		else if (rc1 == 0) {
			printf("child process in loop");
			dup2(fds[1], STDOUT_FILENO);
			dup2(fds[0], STDIN_FILENO);
			int execlp_ret = execlp(argv[i], argv[i], (char *) NULL);
			perror("execlp");
			exit(EXIT_FAILURE);
		}
		else {
			int status = 0;
			waitpid(rc1, &status, 0);
			close(fds[1]);
			printf("Child process in loop exits with code: %d\n", WEXITSTATUS(status));
		}
	}

	// if (pipe(fds)) {
	// 	exit(EXIT_FAILURE);
	// }

	int rc_last = fork();
	if (rc_last < 0) {
		exit(EXIT_FAILURE);
	}
	else if (rc_last == 0) {
		printf("child process after loop");
		dup2(fds[0], STDIN_FILENO);
		int execlp_ret = execlp(argv[argc-1], argv[argc-1], (char *) NULL);
		perror("execlp");
		exit(EXIT_FAILURE);
	}
	else {
		int status = 0;
		waitpid(rc_last, &status, 0);
		close(fds[1]);
		printf("Child process after loop exits with code: %d\n", WEXITSTATUS(status));
	}
	return 0;
}
