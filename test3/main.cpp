#include <pthread.h>
#include <list>
#include <iostream>
#include <string>
#include <sstream>
#include <ctime>
#include <sys/time.h>
#include <errno.h>

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

sig_atomic_t workingNum = 0;

pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

static void* doneTask(void* arg) {
	--workingNum;
	pthread_cond_signal(&cond);
	return NULL;
}

static void* task1(void* arg)
{
	static volatile long res = 1;
	for (long i = 0; i < 1000000; ++i) {
		res ^= (i + 1) * 3 >> 2;
	}
	return doneTask(arg);
}

static void* task2(void* arg)
{
	static volatile long res = 1;
	for (long i = 0; i < 1000000; ++i) {
		res ^= (i + 1) * 3 >> 2;
		if (i % 100000 != 0) {
			continue;
		}
		sched_yield();
	}
	return doneTask(arg);
}

static void* task3(void* arg)
{
	return doneTask(arg);
}

static void* (*task)(void* arg) = NULL;

int main(int argc, char *argv[])
{
	if (argc < 4) {
		return 1;
	}

	auto processNum = 4;
	auto threadNum = atoi(argv[3]);
	auto dtime = atol(argv[2]) * 1000;
	auto taskNum = atoi(argv[1]);
	long cycles = 0;

	switch(taskNum) {
	case 1: task = task1; break;
	case 2: task = task2; break;
	case 3: task = task3; break;
	}

	time_start();

	for (auto i = 0; i < processNum; ++i) {
		if (fork() != 0) {
			continue;
		}

		pthread_attr_t attr;
		pthread_attr_setstacksize(&attr, 64*1024);

		pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
		pthread_mutexattr_t mutexattr;
		pthread_mutex_init(&mutex, &mutexattr);
		pthread_condattr_t condattr;
		pthread_cond_init(&cond, &condattr);

		pthread_t pid;
		while (time_stop() < dtime) {
			for (auto i = workingNum; i < threadNum; ++i) {
				++workingNum;
				if (pthread_create(&pid, NULL, task, NULL) == -1) {
					return 2;
				}
			}

			int rc;
			if ((rc = pthread_mutex_lock(&mutex)) != 0) {
				std::cout << "pthread_mutex_lock " << rc << " " << strerror(rc) << std::endl;
				return 3;
			}
			if (workingNum == threadNum) {
				if ((rc = pthread_cond_wait(&cond, &mutex)) != 0) {
					std::cout << "pthread_cond_wait " << rc << " " << strerror(rc) << std::endl;
					return 3;
				}
			}
			if ((rc = pthread_mutex_unlock(&mutex)) != 0) {
				std::cout << "pthread_mutex_unlock " << rc << " " << strerror(rc) << std::endl;
				return 3;
			}
			//pthread_join(pids.front(), NULL);
			//pids.pop_front();
			cycles += threadNum - workingNum;
		}

		std::cout << cycles << std::endl;

		return 0;
	}
	for (auto i = 0; i < processNum; ++i) {
		wait(NULL);
	}

	return 0;
}
