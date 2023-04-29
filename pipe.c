#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
	//not enough arguments
	if(argc < 2) {
		printf("Not enough arguments\n");
		return(EXIT_FAILURE);
	}

	int PROCESS_NUM = argc - 1;
	int NUM_PIPES = PROCESS_NUM - 1;
	int pipes[NUM_PIPES][2];
	int pids[PROCESS_NUM];
	int i;
	//creating all the pipes
	for(i = 0; i < NUM_PIPES; i++) {
		if(pipe(pipes[i]) == -1) {
			printf("error with creating pipe\n");
			return(EXIT_FAILURE);
		}
	}

	pids[0] = fork();
	if (pids[0] < 0) {
		return(EXIT_FAILURE);
	}

	//first process
	if(pids[0] == 0) {
		dup2(pipes[0][1], STDOUT_FILENO);
		close(pipes[0][0]);
		close(pipes[0][1]);
		execlp(argv[1], argv[1], NULL);
		return(EXIT_FAILURE);
	}

	for(i = 1; i < PROCESS_NUM - 1; i++) {
		pids[i] = fork();
		if(pids[i] < 0) {
			return(EXIT_FAILURE);
		}

		if(pids[i] == 0) {
			dup2(pipes[i-1][0], STDIN_FILENO);
			dup2(pipes[i][1], STDOUT_FILENO);
			close(pipes[i-1][0]);
			close(pipes[i-1][1]);
			close(pipes[i][0]);
			close(pipes[i][1]);
			execlp(argv[i+1], argv[i+1], NULL);
			return(EXIT_FAILURE);
		}
		close(pipes[i-1][0]);
		close(pipes[i-1][1]);
	}
	if(argc != 2) {
		//last process
		pids[PROCESS_NUM-1] = fork();
		if(pids[PROCESS_NUM-1] < 0) {
			return(EXIT_FAILURE);
		}
		if(pids[PROCESS_NUM-1] == 0) {
			dup2(pipes[NUM_PIPES - 1][0], STDIN_FILENO);
			close(pipes[NUM_PIPES - 1][0]);
			close(pipes[NUM_PIPES - 1][1]);
			execlp(argv[PROCESS_NUM], argv[PROCESS_NUM], NULL);
			return(EXIT_FAILURE);
		}

		close(pipes[NUM_PIPES - 1][0]);
		close(pipes[NUM_PIPES - 1][1]);
		
		for(i = 0; i < PROCESS_NUM; i++) {
			waitpid(pids[i], NULL, 0);
		}
	}

	return 0;
}
