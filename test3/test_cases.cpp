#include <map>
#include <string>
#include "test_cases.h"

using namespace std;

typedef void (*InternalTask)(void*);

// arithmetic
void task1(void* arg) {
	static volatile long res = 1;
	for (long i = 0; i < 1000000; ++i) {
		res ^= (i + 1) * 3 >> 2;
	}
}

// context switching
void task2(void* arg) {
	for (long i = 0; i < 100000; ++i) {
		sched_yield();
	}
}

// create/delete
void task3(void* arg) {

}

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

	task = tasks[key];
	doneTask = callback;

	return test;
}
