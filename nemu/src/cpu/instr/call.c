#include "cpu/instr.h"
/*
Put the implementations of `call' instructions here.
*/

make_instr_func(call_near) {
    cpu.eip += 1+data_size/8;
    push_(cpu.eip, data_size);
    OPERAND rel;
    rel.type = OPR_IMM;
    rel.sreg = SREG_CS;
    rel.data_size = data_size;
    rel.addr = eip+1;
    operand_read(&rel);
    cpu.eip = (cpu.eip+rel.val) & (0xFFFFFFFF >> (32-data_size));
    return 0;
}

make_instr_func(call_near_indirect) {
    OPERAND rm;
    rm.data_size = data_size;
    int len = 1;
    len += modrm_rm(eip+1, &rm);
    operand_read(&rm);
    cpu.eip += len;
    push_(cpu.eip, data_size);
    cpu.eip = rm.val & (0xFFFFFFFF >> (32-data_size));
    return 0;
}