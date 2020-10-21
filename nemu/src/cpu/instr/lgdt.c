#include "cpu/instr.h"
/*
Put the implementations of `lgdt' instructions here.
*/
make_instr_func(lgdt){
    int len = 1;
    decode_operand_rm
    cpu.gdtr.base =  opr_src.val;
    return len;
}