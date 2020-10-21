#include "cpu/instr.h"
/*
Put the implementations of `leave' instructions here.
*/
make_instr_func(leave){
    cpu.esp = cpu.ebp;
    OPERAND mem;
    mem.type = OPR_MEM;
    mem.addr = cpu.ebp;
    mem.data_size = data_size;
    mem.sreg = SREG_CS;
    operand_read(&mem);
    cpu.ebp = mem.val;
    cpu.esp += data_size / 8;
    print_asm_0("leave", "", 1);
    return 1;
}