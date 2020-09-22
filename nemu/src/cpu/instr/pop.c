#include "cpu/instr.h"
/*
Put the implementations of `pop' instructions here.
*/
static void instr_execute_1op(){
    opr_src.data_size = data_size;
    opr_src.type = OPR_MEM;
    opr_src.addr = cpu.esp;
    operand_read(&opr_src);
    
    cpu.esp += (data_size / 8);
    opr_dest.val = sign_ext(opr_src.val, opr_src.data_size);
    operand_write(&opr_dest);
}

make_instr_impl_1op(pop, r, v)