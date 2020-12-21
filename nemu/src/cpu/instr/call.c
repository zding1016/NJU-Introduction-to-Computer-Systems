#include "cpu/instr.h"
/*
Put the implementations of `call' instructions here.
*/
make_instr_func(call_near){
    OPERAND src;
    src.type = OPR_IMM;
    src.addr = cpu.eip + 1;
    src.sreg = SREG_SS;
    src.data_size = data_size;
    operand_read(&src);
    
    print_asm_1("call","", 1+data_size/8 ,&src);
    int offset = sign_ext(src.val, data_size);
    
    cpu.esp = cpu.esp - (data_size / 8);
    opr_dest.data_size = data_size;
    opr_dest.type = OPR_MEM;
    opr_dest.sreg = SREG_SS;
    
    opr_dest.addr = cpu.esp;
    eip = eip + 1 + data_size / 8;
    opr_dest.val = eip;
    
    operand_write(&opr_dest);
    cpu.eip = eip + offset;
    return 0;
}

make_instr_func(call_near_indirect){
    int len = 1;
    len += modrm_rm(eip + 1, &opr_src);
    opr_src.sreg = SREG_SS;
    operand_read(&opr_src);
    print_asm_1("call", opr_src.data_size == 8 ? "b" : (opr_src.data_size == 16 ? "w" : "l"), 5, &opr_src);
    cpu.eip += len;
    cpu.esp -= 4;
    opr_dest.val = cpu.eip;
    opr_dest.data_size = data_size;
    opr_dest.addr = cpu.esp;
    opr_dest.type = OPR_MEM;
    opr_dest.sreg = SREG_SS;
    operand_write(&opr_dest);
    cpu.eip = opr_src.val;
    return 0;
}