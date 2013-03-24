#include "pth.h"
#include <list>
#include <iostream>
#include <string>
#include <sstream>
#include <ctime>
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
		pth_yield(NULL);
		/*if (i % 100000 == 0) {
			pth_yield(NULL);
		}
		res ^= (i + 1) * 3 >> 2;*/
	}
	std::stringstream stream;
	stream << pth_self() << ": " << res << "\n";
	auto str = stream.str();
	pth_write(2, str.c_str(), str.size());
	return NULL;
}

int main(int argc, char *argv[])
{
	if (argc < 3) {
		return 1;
	}

	auto threadNum = atoi(argv[2]);
	auto cycles = atol(argv[1]) * 1000000 / threadNum;

	pth_init();

	auto attr = pth_attr_new();
	pth_attr_set(attr, PTH_ATTR_NAME, "task");
	pth_attr_set(attr, PTH_ATTR_STACK_SIZE, 64*1024);
	pth_attr_set(attr, PTH_ATTR_JOINABLE, true);

	time_start();

	std::list<pth_t> pids;
	for (auto i = 0; i < threadNum; ++i) {
		pids.push_back(pth_spawn(attr, task, &cycles));
	}
	void* res;
	while (!pids.empty()) {
		auto pid = pids.front();
		pids.pop_front();
		pth_join(pid, &res);
	}

	std::cout << static_cast<double>(time_stop()) / 1000 << std::endl;

	return 0;
}
