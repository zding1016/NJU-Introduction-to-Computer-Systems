#include "cpu/instr.h"
/*
Put the implementations of `ret' instructions here.
*/
make_instr_func(ret_near) {
    cpu.eip = pop_(data_size) & (0xFFFFFFFF >> (32-data_size));
    return 0;
}

make_instr_func(ret_near_imm16) {
    cpu.eip = pop_(data_size) & (0xFFFFFFFF >> (32-data_size));
    OPERAND imm;
    imm.type = OPR_IMM;
    imm.sreg = SREG_CS;
    imm.data_size = 16;
    imm.addr = eip + 1;
    operand_read(&imm);
    cpu.esp += imm.val;
    return 0;
}