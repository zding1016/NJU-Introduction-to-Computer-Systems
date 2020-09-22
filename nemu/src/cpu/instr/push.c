#include "cpu/instr.h"
/*
Put the implementations of `push' instructions here.
*/

static void instr_execute_1op(){
    OPERAND opr_dest;
    operand_read(&opr_src);
    opr_dest.type = OPR_MEM;
    opr_dest.data_size = data_size;
    opr_dest.val = opr_src.val;
    cpu.esp = cpu.esp - (data_size) / 8;
    opr_dest.addr = cpu.esp;
    operand_write(&opr_dest);
}


make_instr_impl_1op(push, r, v)