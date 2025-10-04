#ifndef _ASSEMBLER_H_
#define _ASSEMBLER_H_

static const size_t CMD_SIZE = 10;

static const char* CMD[CMD_SIZE] = {"HLT",     // 0
                                    "PUSH",    // 1
                                    "POP",     // 2
                                    "ADD",     // 3
                                    "SUB",     // 4
                                    "MUL",     // 5
                                    "DIV",     // 6
                                    "POW",     // 7
                                    "SQRT",    // 8
                                    "OUT"      // 9
                                    };


int Assembler(const char* filename_in, const char* filename_out, size_t* count);

#endif // _ASSEMBLER_H_
