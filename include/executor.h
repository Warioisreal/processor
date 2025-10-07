#ifndef _EXECUTOR_H_
#define _EXECUTOR_H_

#include "../stack/stack.h"

const size_t REGISTERS_COUNT = 8;

typedef enum class PROC_ERROR : char {
    OK                = 0,
    STACK_ERROR       = 1,
    UNKNOWN_COMMAND   = 2,
    BUFFER_NULL_PTR   = 3,
    FILL_BUFFER_ERROR = 4,
    CMD_JUMP_ERROR    = 5,
    PROC_ERR_INVALID_CONDITION = 6
} proc_error_t;

struct Processor {
    int* buffer           = nullptr;
    size_t cmd_count      = 0;
    size_t buffer_size    = 0;
    CREATE_STACK(stack);
    stack_elem_t reg_array[REGISTERS_COUNT] = {POISON};
};


proc_error_t Executor(const char* filename_in);

#endif // _EXECUTOR_H_
