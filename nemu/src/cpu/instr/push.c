#include "cpu/instr.h"
/*
Put the implementations of `push' instructions here.
*/

static void instr_execute_1op(){
    OPERAND dest;
    operand_read(&opr_src);
    dest.type = OPR_MEM;
    dest.data_size = data_size;
    dest.val = opr_src.val;
    cpu.esp = cpu.esp - (data_size) / 8;
    dest.addr = cpu.esp;
    dest.sreg = SREG_SS;
    operand_write(&dest);
}


make_instr_impl_1op(push, r, v)
make_instr_impl_1op(push, rm, v)
make_instr_impl_1op(push, i, b)
make_instr_impl_1op(push, i, v)
make_instr_impl_1op(push, r, w)

make_instr_func(pusha){
	uint32_t t=cpu.esp;
	for(int i=0;i<8;i++){
		OPERAND rel;
		rel.data_size=data_size;
		rel.type=OPR_MEM;
		rel.sreg=SREG_SS;
		cpu.esp-=4;
		rel.addr=cpu.esp;
		if(i==4){
			rel.val=t;
		}
		else{
			rel.val=cpu.gpr[i].val;
		}
		operand_write(&rel);
	}
	return 1;
}