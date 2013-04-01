#include <st.h>
#include <ctime>
#include <sys/time.h>
#include <errno.h>
#include <sys/types.h>
#include <signal.h>
#include <stdlib.h>
#include <iostream>

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
	for (long i = 0; i < 100000; ++i) {
		st_sleep(0); // yield
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

	long processNum = atoi(argv[4]);
	long threadNum = atoi(argv[3]);
	long dtime = atol(argv[2]) * 1000;
	long taskNum = atoi(argv[1]);
	long cycles = 0;

	switch(taskNum) {
	case 1: task = task1; break;
	case 2: task = task2; break;
	case 3: task = task3; break;
	}

	for (long i = 0; i < processNum; ++i) {
		if (fork() != 0) {
			continue;
		}

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
		cycles += workingNum;

		std::cout << ((long double) cycles * 1000) / dtime  << std::endl;

		return 0;
	}
	for (long i = 0; i < processNum; ++i) {
		wait(NULL);
	}

	return 0;
}
