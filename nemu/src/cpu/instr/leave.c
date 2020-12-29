#include "cpu/instr.h"
/*
Put the implementations of `leave' instructions here.
*/
make_instr_func(leave) {
    cpu.esp = cpu.ebp;
    cpu.ebp = pop_(data_size);
    return 1;
}