#include "cpu/instr.h"
/*
Put the implementations of `pop' instructions here.
*/
uint32_t pop_(int datasize) {
    OPERAND rm;
    rm.type = OPR_MEM;
    rm.sreg = SREG_SS;
    rm.addr = cpu.esp;
    rm.data_size = datasize;
    operand_read(&rm);
    cpu.esp += datasize >> 3;
    return rm.val;
}

static void instr_execute_1op() {
    opr_src.val = pop_(opr_src.data_size);
    operand_write(&opr_src);
}

make_instr_impl_1op(pop, rm, v)
make_instr_impl_1op(pop, r, v)

make_instr_func(popa) {
    int len = 1;
    for (int i = 7; i >= 0; --i) {
        uint32_t val = pop_(data_size);
        if (i == 4) continue;
        if (data_size == 16) cpu.gpr[i]._16 = val;
        else cpu.gpr[i]._32 = val;
    }
    return len;
}