#include "cpu/instr.h"
/*
Put the implementations of `pop' instructions here.
*/
static void instr_execute_1op(){
    opr_dest.data_size = data_size;
    opr_dest.type = OPR_MEM;
    opr_dest.addr = cpu.esp;
    opr_dest.sreg = SREG_CS;
    operand_read(&opr_dest);
    
    cpu.esp += (32 / 8);
    opr_src.val = opr_dest.val;
    opr_src.sreg = SREG_SS;
    operand_write(&opr_src);
}

make_instr_impl_1op(pop, r, v)
make_instr_impl_1op(pop, rm, v)
make_instr_impl_1op(pop, i, b)
make_instr_impl_1op(pop, i, v)

make_instr_func(popa){
	OPERAND reg[8];
	int len = 1;

	for(int i=0;i<8;i++){
		reg[i].data_size = data_size;
		reg[i].type = OPR_MEM;
		reg[i].sreg = SREG_SS;
	}
	for(int i=7;i>=0;i--){
		reg[i].addr = cpu.esp;
		cpu.esp += data_size/8;
		operand_read(&reg[i]);
	}
	for(int i=0;i<8;i++){
		reg[i].type = OPR_REG;
		reg[i].addr = i;
		if(i!=REG_ESP)
			operand_write(&reg[i]);
	}
	return len;
}