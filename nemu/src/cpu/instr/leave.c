#include "cpu/instr.h"
/*
Put the implementations of `leave' instructions here.
*/
make_instr_func(leave){
    cpu.esp = cpu.ebp;
    OPERAND mem;
    mem.type = OPR_MEM;
    mem.addr = cpu.ebp;
    mem.data_size = data_size;
    operand_read(&mem);
    cpu.ebp = mem.val;
    cpu.esp += data_size / 8;
    return 1;
}