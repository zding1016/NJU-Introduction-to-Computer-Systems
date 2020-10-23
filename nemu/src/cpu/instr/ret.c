#include "cpu/instr.h"
/*
Put the implementations of `ret' instructions here.
*/
make_instr_func(ret_near){
    OPERAND src;
    src.type = OPR_MEM;
    src.data_size = 32;
    src.sreg = SREG_CS;
    src.addr = cpu.esp;
    operand_read(&src);
    print_asm_0("ret","",1);
    cpu.esp += 32 / 8;
    cpu.eip = src.val;
    return 0;
}

make_instr_func(ret_near_imm16){
    print_asm_0("ret","",3);
    OPERAND mem;
    mem.type = OPR_MEM;
    mem.data_size = 32;
    mem.sreg = SREG_CS;
    mem.addr = cpu.esp;
    operand_read(&mem);
    cpu.eip = mem.val;
    int offset = sign_ext(instr_fetch(eip + 1, 2), 16);
    cpu.esp += offset + 16 / 8;
    return 0;
}