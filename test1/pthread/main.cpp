#include <pthread.h>
#include <list>
#include <iostream>
#include <time.h>
#include <fcntl.h>

#include <sys/time.h>
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

static void* task(void* arg)
{
	auto cycles = *static_cast<long*>(arg);
	long res = 1;
	auto fd = open("/dev/null", O_WRONLY);
	for (long i = 0; i < cycles; ++i) {
		sched_yield();
		/*if (i % 100000 == 0) {
			sched_yield();
		}
		res ^= (i + 1) * 3 >> 2;*/
	}
	std::cout << pthread_self() << ": " << res << std::endl;
	return NULL;
}

int main(int argc, char *argv[])
{
	if (argc < 3) {
		return 1;
	}

	auto threadNum = atoi(argv[2]);
	auto cycles = atol(argv[1]) * 1000000 / threadNum;

	time_start();

	std::list<pthread_t> pids;
	for (auto i = 0; i < threadNum; ++i) {
		pids.push_back(pthread_t());
		if (pthread_create(&pids.back(), NULL, task, &cycles) == -1) {
			return 2;
		}
	}
	void* res;
	while (!pids.empty()) {
		pthread_join(pids.front(), &res);
		pids.pop_front();
	}

	std::cout << static_cast<double>(time_stop()) / 1000 << std::endl;

	return 0;
}
