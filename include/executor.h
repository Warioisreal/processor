#ifndef _EXECUTOR_H_
#define _EXECUTOR_H_

#include "../stack/stack.h"

const size_t REGISTERS_COUNT = 16;

struct Processor {
    int* buffer           = nullptr;
    size_t cmd_count      = 0;
    size_t buffer_size    = 0;
    CREATE_STACK(stack);
    stack_elem_t reg_array[REGISTERS_COUNT] = {POISON};
};


int Executor(const char* filename_in, size_t count);

#endif // _EXECUTOR_H_
