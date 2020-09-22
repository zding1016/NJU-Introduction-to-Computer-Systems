#include "cpu/instr.h"
/*
Put the implementations of `call' instructions here.
*/
make_instr_func(call_near){
    OPERAND src;
    src.type = OPR_IMM;
    src.addr = eip + 1;
    src.data_size = data_size;
    operand_read(&src);
    
    print_asm_1("call","", 1+data_size/8 ,&src);
    int offset = sign_ext(src.val, data_size);
    
    cpu.esp = cpu.esp - (data_size / 8);
    opr_dest.data_size = data_size;
    opr_dest.type = OPR_MEM;
    
    opr_dest.addr = cpu.esp;
    eip = eip + 1 + data_size / 8;
    opr_dest.val = eip;
    
    operand_write(&opr_dest);
    cpu.eip = eip + offset;
    return 0;
}