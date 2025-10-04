#include <math.h>
#include <assert.h>

#include "../stack/stack.h"
#include "color_lib.h"

#include "executor.h"

static int ReadDataOToBuffer(struct Processor* proc, const char* filename);


int Executor(const char* filename_in, size_t count) {
    assert (filename_in != nullptr);

    struct Processor proc = {.buffer_size = count};

    if (ReadDataOToBuffer(&proc, filename_in)) { return 1; }

    stack_elem_t value1 = POISON;
    stack_elem_t value2 = POISON;
    CHECK_ERROR(StackCtor(&(proc.stack), 16));

    size_t pos = 0;
    while (proc.buffer[pos] != 0) {
        switch(proc.buffer[pos]) {
            case 1: // PUSH
                pos++;
                CHECK_ERROR(StackPush(&(proc.stack), proc.buffer[pos]));
                break;
            case 2: // POP
                CHECK_ERROR(StackPop(&(proc.stack), &value1));
                break;
            case 3: // ADD
                CHECK_ERROR(StackPop(&(proc.stack), &value2));
                CHECK_ERROR(StackPop(&(proc.stack), &value1));
                CHECK_ERROR(StackPush(&(proc.stack), value1 + value2));
                break;
            case 4: // SUB
                CHECK_ERROR(StackPop(&(proc.stack), &value2));
                CHECK_ERROR(StackPop(&(proc.stack), &value1));
                CHECK_ERROR(StackPush(&(proc.stack), value1 - value2));
                break;
            case 5: // MUL
                CHECK_ERROR(StackPop(&(proc.stack), &value2));
                CHECK_ERROR(StackPop(&(proc.stack), &value1));
                CHECK_ERROR(StackPush(&(proc.stack), value1 * value2));
                break;
            case 6: // DIV
                CHECK_ERROR(StackPop(&(proc.stack), &value2));
                CHECK_ERROR(StackPop(&(proc.stack), &value1));
                CHECK_ERROR(StackPush(&(proc.stack), value1 / value2));
                break;
            case 7: // POW
                CHECK_ERROR(StackPop(&(proc.stack), &value2));
                CHECK_ERROR(StackPop(&(proc.stack), &value1));
                CHECK_ERROR(StackPush(&(proc.stack), pow(value1, value2)));
                break;
            case 8: // SQRT
                CHECK_ERROR(StackPop(&(proc.stack), &value1));
                CHECK_ERROR(StackPush(&(proc.stack), sqrt(value1)));
                break;
            case 9: // OUT
                CHECK_ERROR(StackPop(&(proc.stack), &value1));
                PRINT_STACK_ELEMENT(BASE, value1);
                break;
            default: break;
        }
        pos++;
    }
    return 0;
}



static int ReadDataOToBuffer(struct Processor* proc, const char* filename) {
    assert (proc     != nullptr);
    assert (filename != nullptr);

    FILE* file = fopen(filename, "rb");
    if (file == nullptr) {
        PRINT_COLOR_VAR(RED, "file open error: \"%s\"\n", filename);
        return 1;
    }

    int* buf_ = (int*)calloc(proc->buffer_size + 1, sizeof(int));
    if (buf_ == nullptr) {
        PRINT_COLOR(RED, "buffer calloc error\n");
        return 1;
    }
    proc->buffer = buf_;

    int cmd = 0;
    for (size_t pos = 0; pos < proc->buffer_size; pos++) {
        if (fscanf(file, "%d", &cmd)) {
            proc->buffer[pos] = cmd;
        } else {
            PRINT_COLOR_VAR(RED, "fscanf error on cmd: %zu\n", pos);
            return 1; // error
        }
    }

    fclose(file);

    return 0;
}
