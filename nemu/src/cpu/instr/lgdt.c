#include "cpu/instr.h"
/*
Put the implementations of `lgdt' instructions here.
*/
make_instr_func(lgdt){
    OPERAND rel;
    int len = 1;
    len += modrm_rm(eip + 1, &rel);
    rel.data_size = 16;
    operand_read(&rel);
    cpu.gdtr.limit = rel.val;
    rel.addr += 2;
    rel.data_size = 32;
    operand_read(&rel);
    cpu.gdtr.base = rel.val;
    print_asm_1("lgdt", "", len, &rel);
    return len;
}