#include "cpu/instr.h"
/*
Put the implementations of `lgdt' instructions here.
*/
make_instr_func(lgdt) {
    int len = 1;
#ifdef IA32_SEG
    OPERAND rm;
    rm.data_size = 16;
    rm.sreg = SREG_DS;
    len += modrm_rm(eip+1, &rm);
    operand_read(&rm);
    cpu.gdtr.limit = rm.val & 0x0000FFFF;
    rm.data_size = 32;
    rm.addr += 2;
    operand_read(&rm);
    cpu.gdtr.base = rm.val & (data_size == 16 ? 0x00FFFFFF : 0xFFFFFFFF);
#endif
    return len;
}