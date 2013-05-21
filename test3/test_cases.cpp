#include <map>
#include <string>
#include "test_cases.h"

using namespace std;

typedef void (*InternalTask)(void*);

// arithmetic
void task1(void* arg) {
	volatile long res = 1;
	for (long i = 0; i < 10000000; ++i) {
		res ^= (i + 1) * 3 >> 2;
	}
}

// context switching
extern void task2(void* arg);

// create/delete
void task3(void* arg) {

}

// mutex
extern void task4(void* arg);

// long calculations
void task5(void* arg) {
	volatile long res = 1;
	for (long i = 0; i < 1000000000; ++i) {
		res ^= (int)(((i + 1) * 3 >> 2) / 0.66);
	}
}

// shared atomic memory
void task6(void* arg) {
	static volatile long res = 1;
	for (long i = 0; i < 10000000; ++i) {
		res ^= (i + 1) * 3 >> 2;
	}
}

// read/write without call multiplexing
extern void task7(void* arg);

// read/write with call multiplexing
extern void task8(void* arg);

static InternalTask task;
static Task doneTask;

static void* test(void* arg)
{
	task(arg);
	return doneTask(arg);
}

Task get_test(const char* key, Task callback)
{
	map<string, InternalTask> tasks;
	tasks["1"] = task1;
	tasks["2"] = task2;
	tasks["3"] = task3;
	tasks["4"] = task4;
	tasks["5"] = task5;
	tasks["6"] = task6;
	tasks["7"] = task7;
	tasks["8"] = task8;

	task = tasks[key];
	doneTask = callback;

	return test;
}
