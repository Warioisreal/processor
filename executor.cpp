#include <math.h>
#include <assert.h>

#include "../stack/stack.h"
#include "../assembler/asm.h"
#include "color_lib.h"

#include "executor.h"
#include "work_with_buffer.h"

static int ReadDataOToBuffer(struct Processor* proc, const char* filename);
static stack_error_t StackAdd(stack_type* stack);
static stack_error_t StackSub(stack_type* stack);
static stack_error_t StackMul(stack_type* stack);
static stack_error_t StackDiv(stack_type* stack);
static stack_error_t StackPow(stack_type* stack);
static stack_error_t StackSqrt(stack_type* stack);
static proc_error_t StackJMP(struct Processor* proc);

enum class Condition : char {
    COND_ABOVE       = 0,   // JA
    COND_ABOVE_EQUAL = 1,   // JAE
    COND_BELOW       = 2,   // JB
    COND_BELOW_EQUAL = 3,   // JBE
    COND_EQUAL       = 4,   // JE
    COND_NOT_EQUAL   = 5    // JNE
};

static proc_error_t StackJMPCond(struct Processor* proc, Condition cond);

void PrintReg(const stack_elem_t reg_array[REGISTERS_COUNT]);

#define CHECK_STACK_COMMAND(error) if (error != stack_error_t::OK) { return error; }
#define CHECK_STACK_ERROR(error) if (error != stack_error_t::OK) { return proc_error_t::STACK_ERROR; }


proc_error_t Executor(const char* filename_in) {
    assert (filename_in != nullptr);

    struct Processor proc = {};

    if (ReadDataOToBuffer(&proc, filename_in)) { return proc_error_t::FILL_BUFFER_ERROR; }

    stack_elem_t value = POISON;
    CHECK_STACK_ERROR(StackCtor(&(proc.stack), 16));

    int run = 1;

    while (run) {
        printf("command: %s\n", CMD_ARRAY[proc.buffer[proc.cmd_count]].text_cmd);

        switch(static_cast<CMD>(proc.buffer[(proc.cmd_count)++])) {
            case CMD::CMD_HLT:
                run = 0;
                break;
            case CMD::CMD_PUSH:
                CHECK_STACK_ERROR(StackPush(&(proc.stack), proc.buffer[(proc.cmd_count)++]));
                break;
            case CMD::CMD_POP:
                CHECK_STACK_ERROR(StackPop(&(proc.stack), &value));
                break;
            case CMD::CMD_ADD:
                CHECK_STACK_ERROR(StackAdd(&(proc.stack)));
                break;
            case CMD::CMD_SUB:
                CHECK_STACK_ERROR(StackSub(&(proc.stack)));
                break;
            case CMD::CMD_MUL:
                CHECK_STACK_ERROR(StackMul(&(proc.stack)));
                break;
            case CMD::CMD_DIV:
                CHECK_STACK_ERROR(StackDiv(&(proc.stack)));
                break;
            case CMD::CMD_POW:
                CHECK_STACK_ERROR(StackPow(&(proc.stack)));
                break;
            case CMD::CMD_SQRT:
                CHECK_STACK_ERROR(StackSqrt(&(proc.stack)));
                break;
            case CMD::CMD_IN:
                ENTER_STACK_ELEMENT(&value);
                CHECK_STACK_ERROR(StackPush(&(proc.stack), value));
                break;
            case CMD::CMD_OUT:
                CHECK_STACK_ERROR(StackPop(&(proc.stack), &value));
                PRINT_COLOR(CYAN, "OUT: ");
                PRINT_STACK_ELEMENT(BASE, value);
                break;
            case CMD::CMD_JMP:
                StackJMP(&proc);
                break;
            case CMD::CMD_JB:
                StackJMPCond(&proc, Condition::COND_BELOW);
                break;
            case CMD::CMD_JBE:
                StackJMPCond(&proc, Condition::COND_BELOW_EQUAL);
                break;
            case CMD::CMD_JA:
                StackJMPCond(&proc, Condition::COND_ABOVE);
                break;
            case CMD::CMD_JAE:
                StackJMPCond(&proc, Condition::COND_ABOVE_EQUAL);
                break;
            case CMD::CMD_JE:
                StackJMPCond(&proc, Condition::COND_EQUAL);
                break;
            case CMD::CMD_JNE:
                StackJMPCond(&proc, Condition::COND_NOT_EQUAL);
                break;
            case CMD::CMD_PUSHR:
                CHECK_STACK_ERROR(StackPush(&(proc.stack), proc.reg_array[proc.buffer[(proc.cmd_count)++]]));
                break;
            case CMD::CMD_POPR:
                CHECK_STACK_ERROR(StackPop(&(proc.stack), &(proc.reg_array[proc.buffer[(proc.cmd_count)++]])));
                break;
            default:
                PRINT_COLOR(RED, "Unknown command\n");
                return proc_error_t::UNKNOWN_COMMAND;
        }

        printf("cmd_count: %zu\n", proc.cmd_count);
        PrintStack(&(proc.stack));
        PrintReg(proc.reg_array);
        printf("____________________________________\n");
        getchar();

    }

    CHECK_STACK_ERROR(StackDtor(&(proc.stack)));
    free(proc.buffer);
    proc.buffer      = nullptr;
    proc.buffer_size = 0;
    proc.cmd_count   = 0;
    return proc_error_t::OK;
}



static int ReadDataOToBuffer(struct Processor* proc, const char* filename) {
    assert (proc     != nullptr);
    assert (filename != nullptr);

    FILE* file = fopen(filename, "rb");
    if (file == nullptr) {
        PRINT_COLOR_VAR(RED, "file open error: \"%s\"\n", filename);
        return 1;
    }
    size_t count = 0;
    if(fscanf(file, "%zu", &count)) {
        proc->buffer_size = count;
    } else {
        PRINT_COLOR_VAR(RED, "buffer_size read error: %zu\n", count);
        return 1;
    }


    int* buf_ = (int*)calloc(count + 1, sizeof(int));
    if (buf_ == nullptr) {
        PRINT_COLOR(RED, "buffer calloc error\n");
        return 1;
    }
    proc->buffer = buf_;

    int cmd = 0;
    for (size_t pos = 0; pos < count; pos++) {
        if (fscanf(file, "%d", &cmd)) {
            proc->buffer[pos] = cmd;
        } else {
            PRINT_COLOR_VAR(RED, "fscanf error on cmd: %zu\n", pos);
            return 1;
        }
    }

    fclose(file);

    return 0;
}


static stack_error_t StackAdd(stack_type* stack) {
    stack_elem_t value1 = POISON;
    stack_elem_t value2 = POISON;
    CHECK_STACK_COMMAND(StackPop(stack, &value2));
    CHECK_STACK_COMMAND(StackPop(stack, &value1));
    CHECK_STACK_COMMAND(StackPush(stack, value1 + value2));
    return stack_error_t::OK;
}

static stack_error_t StackSub(stack_type* stack) {
    stack_elem_t value1 = POISON;
    stack_elem_t value2 = POISON;
    CHECK_STACK_COMMAND(StackPop(stack, &value2));
    CHECK_STACK_COMMAND(StackPop(stack, &value1));
    CHECK_STACK_COMMAND(StackPush(stack, value1 - value2));
    return stack_error_t::OK;
}

static stack_error_t StackMul(stack_type* stack) {
    stack_elem_t value1 = POISON;
    stack_elem_t value2 = POISON;
    CHECK_STACK_COMMAND(StackPop(stack, &value2));
    CHECK_STACK_COMMAND(StackPop(stack, &value1));
    CHECK_STACK_COMMAND(StackPush(stack, value1 * value2));
    return stack_error_t::OK;
}

static stack_error_t StackDiv(stack_type* stack) {
    stack_elem_t value1 = POISON;
    stack_elem_t value2 = POISON;
    CHECK_STACK_COMMAND(StackPop(stack, &value2));
    CHECK_STACK_COMMAND(StackPop(stack, &value1));
    CHECK_STACK_COMMAND(StackPush(stack, value1 / value2));
    return stack_error_t::OK;
}

static stack_error_t StackPow(stack_type* stack) {
    stack_elem_t value1 = POISON;
    stack_elem_t value2 = POISON;
    CHECK_STACK_COMMAND(StackPop(stack, &value2));
    CHECK_STACK_COMMAND(StackPop(stack, &value1));
    CHECK_STACK_COMMAND(StackPush(stack, pow(value1, value2)));
    return stack_error_t::OK;
}

static stack_error_t StackSqrt(stack_type* stack) {
    stack_elem_t value = POISON;
    CHECK_STACK_COMMAND(StackPop(stack, &value));
    CHECK_STACK_COMMAND(StackPush(stack, sqrt(value)));
    return stack_error_t::OK;
}

static proc_error_t StackJMP(struct Processor* proc) {
    proc->cmd_count = (size_t)(proc->buffer[proc->cmd_count]);
    return proc_error_t::OK;
}

static proc_error_t StackJMPCond(struct Processor* proc, Condition cond) {
    stack_elem_t value1 = POISON;
    stack_elem_t value2 = POISON;
    CHECK_STACK_ERROR(StackPop(&(proc->stack), &value2));
    CHECK_STACK_ERROR(StackPop(&(proc->stack), &value1));

    bool result = false;
    switch (cond) {
        case Condition::COND_ABOVE:
            result = (value1 > value2);
            break;
        case Condition::COND_ABOVE_EQUAL:
            result = (value1 >= value2);
            break;
        case Condition::COND_BELOW:
            result = (value1 < value2);
            break;
        case Condition::COND_BELOW_EQUAL:
            result = (value1 <= value2);
            break;
        case Condition::COND_EQUAL:
            result = (value1 == value2);
            break;
        case Condition::COND_NOT_EQUAL:
            result = (value1 != value2);
            break;
        default:
            return proc_error_t::PROC_ERR_INVALID_CONDITION;
    }

    if (result) {
        if (StackJMP(proc) != proc_error_t::OK) {
            return proc_error_t::CMD_JUMP_ERROR;
        }
    } else {
        proc->cmd_count++;
    }
    return proc_error_t::OK;
}

void PrintReg(const stack_elem_t reg_array[REGISTERS_COUNT]) {
    PRINT_COLOR(YELLOW, "reg_array:\n");
    for (size_t pos = 0; pos < REGISTERS_COUNT; pos++) {
        PRINT_COLOR_VAR(BASE, "    [%zu] = ", pos);
        PRINT_STACK_ELEMENT(BASE, reg_array[pos]);
    }
    printf("\n");
}
