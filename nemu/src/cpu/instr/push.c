#include "cpu/instr.h"
/*
Put the implementations of `push' instructions here.
*/

static void instr_execute_1op(){
    OPERAND dest;
    oprand_read(&opr_src);
    dest.type = OPR_MEM;
    dest.data_size = data_size;
    cpu.eip = cpu.eip - (data_size) / 8;
    dest.addr = cpu.eip;
    oprand_write(&dest);
}


make_instr_impl_1op(push, r, v);
make_instr_impl_1op(push, rm, v);
make_instr_impl_1op(push, i, b);
make_instr_impl_1op(push, i, v);