#include <stdio.h>

#include "file_func.h"
#include "executor.h"


int main(void) {
    char data_o_filename[MAX_FILENAME_LEN] = "";

    GetFileName(data_o_filename, DEFAULT_FILENAME_DATA_O);

    if (Executor(data_o_filename, cmd_count)) { return 1; }

    return 0;
}
