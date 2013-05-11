#include <pthread.h>
#include <ctime>
#include <sys/time.h>
#include <errno.h>
#include <sys/types.h>
#include <signal.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <stdio.h>
#include <limits.h>
#include <sys/wait.h>
#include "test_cases.h"

struct timeval tv1,tv2,dtv;
struct timezone tz;
void time_start() { gettimeofday(&tv1, &tz); }
long time_stop()
{ gettimeofday(&tv2, &tz);
  dtv.tv_sec= tv2.tv_sec -tv1.tv_sec;
  dtv.tv_usec=tv2.tv_usec-tv1.tv_usec;
  if(dtv.tv_usec<0) { dtv.tv_sec--; dtv.tv_usec+=1000000; }
  return dtv.tv_sec*1000+dtv.tv_usec/1000;
}

long workingNum = 0;

pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

static void* doneTask(void* arg) {
	int rc = 0;
	if ((rc = pthread_mutex_lock(&mutex)) != 0) {
		std::cout << "pthread_mutex_lock " << rc << " " << strerror(rc) << std::endl;
		return NULL;
	}
	--workingNum;
	if ((rc = pthread_cond_signal(&cond)) != 0) {
		std::cout << "pthread_cond_signal " << rc << " " << strerror(rc) << std::endl;
		return NULL;
	}
	if ((rc = pthread_mutex_unlock(&mutex)) != 0) {
		std::cout << "pthread_mutex_unlock " << rc << " " << strerror(rc) << std::endl;
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

	for (long i = 0; i < processNum; ++i) {
		if (fork() != 0) {
			continue;
		}

		pthread_attr_t attr;
		pthread_attr_init(&attr);
		pthread_attr_setstacksize(&attr, PTHREAD_STACK_MIN);

		pthread_mutexattr_t mutexattr;
		pthread_mutex_init(&mutex, &mutexattr);

		pthread_condattr_t condattr;
		pthread_cond_init(&cond, &condattr);

		int rc = 0;
		pthread_t pid;

		if ((rc = pthread_mutex_lock(&mutex)) != 0) {
			std::cout << "pthread_mutex_lock " << rc << " " << strerror(rc) << std::endl;
			return 3;
		}

		time_start();
		while (time_stop() < dtime) {
			for (long i = workingNum; i < threadNum; ++i) {
				if ((rc = pthread_create(&pid, &attr, task, NULL)) != 0) {
					std::cout << "pthread_create " << rc << " " << strerror(rc) << std::endl;
					return 2;
				}
				++workingNum;
			}

			if (workingNum == threadNum) {
				if ((rc = pthread_cond_wait(&cond, &mutex)) != 0) {
					std::cout << "pthread_cond_wait " << rc << " " << strerror(rc) << std::endl;
					return 3;
				}
			}
			//pthread_join(pids.front(), NULL);
			//pids.pop_front();
			cycles += threadNum - workingNum;
		}
		//cycles += workingNum;

		if ((rc = pthread_mutex_unlock(&mutex)) != 0) {
			std::cout << "pthread_mutex_unlock " << rc << " " << strerror(rc) << std::endl;
			return 3;
		}

		std::cout << ((long double) cycles * 1000) / dtime  << std::endl;

		return 0;
	}
	for (long i = 0; i < processNum; ++i) {
		wait(NULL);
	}

	return 0;
}
