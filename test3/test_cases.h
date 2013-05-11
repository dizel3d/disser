#pragma once

typedef void* (*Task)(void*);

Task get_test(const char* key, Task callback);
