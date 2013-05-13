#include <st.h>
#include <errno.h>
#include <sys/types.h>
#include <signal.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <stdio.h>
#include <limits.h>
#include <sys/wait.h>
#include "timer.h"
#include "test_cases.h"

long workingNum = 0;

st_cond_t cond = st_cond_new();

static void* doneTask(void* arg) {
	int rc = 0;
	--workingNum;
	if ((rc = st_cond_signal(cond)) != 0) {
		std::cout << "pthread_cond_signal " << rc << " " << strerror(rc) << std::endl;
		return NULL;
	}
	return NULL;
}

int main(int argc, char *argv[])
{
	if (argc < 4) {
		return 1;
	}

	long processNum = atoi(argv[4]);
	long threadNum = atoi(argv[3]);
	long dtime = atol(argv[2]) * 1000;
	char* testName = argv[1];
	long cycles = 0;
	Task task = get_test(testName, doneTask);
	int pipefd[2];

	// open pipe
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

	for (long i = 0; i < processNum; ++i) {
		if (fork() != 0) {
			continue;
		}

		// close unused read end
		close(pipefd[0]);

		st_init();

		int rc = 0;
		st_thread_t pid;

		time_start();
		while (time_stop() < dtime) {
			for (long i = workingNum; i < threadNum; ++i) {
				if ((pid = st_thread_create(task, NULL, false, PTHREAD_STACK_MIN)) == NULL) {
					std::cout << "pthread_create " << rc << " " << strerror(rc) << std::endl;
					return 2;
				}
				++workingNum;
			}

			if (workingNum == threadNum) {
				if ((rc = st_cond_wait(cond)) != 0) {
					std::cout << "pthread_cond_wait " << rc << " " << strerror(rc) << std::endl;
					return 3;
				}
			}
			//pthread_join(pids.front(), NULL);
			//pids.pop_front();
			cycles += threadNum - workingNum;
		}
		//cycles += workingNum;

		write(pipefd[1], &cycles, sizeof(cycles));
		close(pipefd[1]);

		return 0;
	}

	// close unused write end
	close(pipefd[1]);

	long double result = 0;
	while (read(pipefd[0], &cycles, sizeof(cycles)) > 0) {
		result += cycles;
	}
	close(pipefd[0]);

	std::cout << ((long double) result * 1000) / dtime << std::endl;

	return 0;
}
