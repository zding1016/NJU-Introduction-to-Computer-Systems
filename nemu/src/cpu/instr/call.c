#include "cpu/instr.h"
/*
Put the implementations of `call' instructions here.
*/
make_instr_func(call_near){
    OPERAND src;
    src.type = OPR_IMM;
    src.addr = eip + 1;
    src.sreg = SREG_CS;
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
    OPERAND rel, mem;
    rel.data_size = data_size;
    rel.sreg = SREG_CS;
    len += modrm_rm(eip + 1, &rel);
    operand_read(&rel);
    print_asm_1("call","",len, &rel);
    cpu.esp = cpu.esp - data_size / 8;
    mem.data_size = data_size;
    mem.type = OPR_MEM;
    mem.sreg = SREG_SS;
    mem.addr = cpu.esp;
    mem.val = cpu.eip + len;
    operand_write(&mem);
    if (data_size == 16)
        cpu.eip = rel.val &0xffff;
    else
        cpu.eip = rel.val;
    return 0;
}