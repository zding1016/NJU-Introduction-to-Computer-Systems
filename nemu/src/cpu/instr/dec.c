#include "cpu/instr.h"
/*
Put the implementations of `dec' instructions here.
*/
static void instr_execute_1op(){
    operand_read(&opr_src);
    opr_src.val = alu_sub(1, opr_src.val, data_size);
    operand_write(&opr_src);
}

make_instr_impl_2op(dec, r, v)