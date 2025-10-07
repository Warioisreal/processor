#include <stdio.h>

#include "../assembler/asm.h"

#include "file_func.h"
#include "executor.h"


int main(void) {
    char full_path[MAX_FILENAME_LEN] = "";

    GetFilePath(full_path, DEFAULT_FILENAME_DATA_O);

    if (Executor(full_path) != proc_error_t::OK) { return 1; }

    return 0;
}
