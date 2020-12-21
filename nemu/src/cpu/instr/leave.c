#include "cpu/instr.h"
/*
Put the implementations of `leave' instructions here.
*/
make_instr_func(leave){
    cpu.esp = cpu.ebp;
    opr_dest.type = OPR_MEM;
    opr_dest.addr = cpu.esp;
    opr_dest.data_size = 32;
    opr_dest.sreg = SREG_SS;
    operand_read(&opr_dest);
    print_asm_1("leave", "", 1, &opr_dest);
    cpu.ebp = opr_dest.val;
    cpu.esp += 4;
    return 1;
}