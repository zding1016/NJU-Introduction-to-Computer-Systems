#include "cpu/instr.h"
/*
Put the implementations of `call' instructions here.
*/
make_instr_func(call){
    OPERAND src, dest;
    src.type = OPR_IMM;
    src.addr = cup.eip + 1;
    src.data_size = data_size;
    operand_read(src);
    cpu.esp = cpu.esp - (data_size / 8);
    dest.data_size = data_size;
    dest.type = OPR_MEM;
    dest.addr = cpu.esp;
    dest.val = cpu.eip + 1 + data_size / 8;
    operand_write(&dest);
    cpu.eip = cpu.eip + src.val + 1 + data_size / 8;
}