#include "cpu/instr.h"
/*
Put the implementations of `ret' instructions here.
*/
make_instr_func(ret){
    OPERAND src;
    src.type = OPR_MEM;
    src.data_size = data_size;
    src.addr = cpu.esp;
    operand_read(&src);
    cpu.eip = src.val;
    cpu.esp = cpu.esp - (data_size / 8);
    return 0;
}