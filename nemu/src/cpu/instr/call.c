#include "cpu/instr.h"
/*
Put the implementations of `call' instructions here.
*/
/*make_instr_func(call_near){
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
    cpu.esp = cpu.esp - (32 / 8);
    opr_dest.addr = cpu.esp;
    eip = eip + 1 + data_size / 8;
    opr_dest.val = eip;
    
    operand_write(&opr_dest);
    cpu.eip = eip + offset;
    return 0;
}*/
make_instr_func(call_near) {
	OPERAND rel,mem;
	rel.data_size=data_size;
    rel.type=OPR_IMM;
	//rel.sreg=SREG_CS;
    rel.addr=cpu.eip+1;
    operand_read(&rel);
	print_asm_1("call", "", 1+data_size/8, &rel);
	cpu.esp-=data_size/8;
	mem.data_size=data_size;
	mem.type=OPR_MEM;
	//mem.sreg=SREG_DS;
	mem.addr=cpu.esp;
	mem.val=cpu.eip+1+data_size/8;
	operand_write(&mem);
	int offset=sign_ext(rel.val, data_size);
	cpu.eip+=offset+1+data_size/8;
	return 0;
}