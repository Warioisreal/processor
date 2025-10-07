#include <string.h>
#include <assert.h>

#include "color_lib.h"

#include "work_with_buffer.h"


void FreeBuffer(int** buffer) {

    assert (buffer != nullptr);

    free(*buffer);
    *buffer = nullptr;
}

