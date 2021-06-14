#include "cpu/instr.h"
/*
Put the implementations of `push' instructions here.
*/
void push_(uint32_t val, int datasize) {
    OPERAND rm;
    cpu.esp -= datasize == 16 ? 2 : 4;
    rm.type = OPR_MEM;
    rm.sreg = SREG_SS;
    rm.data_size = datasize == 16 ? 16 : 32;
    rm.addr = cpu.esp;
    rm.val = val;
    operand_write(&rm);
}

static void instr_execute_1op() {
    operand_read(&opr_src);
    push_(sign_ext(opr_src.val, opr_src.data_size), opr_src.data_size);
}

make_instr_impl_1op(push, rm, v)
make_instr_impl_1op(push, r, v)
make_instr_impl_1op(push, i, b)
make_instr_impl_1op(push, i, v)

make_instr_func(pusha) {
    int len = 1;
    for (int i = 0; i < 8; ++i) {
        push_(sign_ext(cpu.gpr[i].val, data_size), data_size);
    }
    return len;
}