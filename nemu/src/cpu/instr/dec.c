#include "cpu/instr.h"
/*
Put the implementations of `dec' instructions here.
*/
static void instr_execute_1op(){
    operand_read(&opr_src);
    print_asm_1("dec","",1+opr_src.data_size/8,&opr_src);
    bool tempcf = cpu.eflags.CF;
    opr_src.val = alu_sub(1,sign_ext(opr_src.val,opr_src.data_size),opr_src.data_size);
    cpu.eflags.CF = tempcf;
    operand_write(&opr_src);
}

make_instr_impl_1op(dec, r, v)
make_instr_impl_1op(dec, rm, v)