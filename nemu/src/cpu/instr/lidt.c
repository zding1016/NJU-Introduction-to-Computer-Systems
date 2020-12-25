#include "cpu/instr.h"
/*
Put the implementations of `lidt' instructions here.
*/
make_instr_func(lidt)
{
	/*OPERAND t;
	t.data_size = data_size;
	int len = 1;
	len=len + modrm_rm(eip + 1, &t);
	cpu.idtr.limit=laddr_read(t.addr,2);
	if(data_size==32){
		cpu.idtr.base=laddr_read(t.addr+2,4);
	}
	else{
		cpu.idtr.base=laddr_read(t.addr+2,3);
	}
	return len;*/
	opr_src.data_size = 16;   //16
    opr_dest.data_size = 32;  //32
    int len = 1;
    len += modrm_rm(eip + 1, &opr_src);
    opr_dest.type = opr_src.type;
    opr_dest.addr = opr_src.addr + 2;
    operand_read(&opr_src);
    operand_read(&opr_dest);
    print_asm_1("lidt", "", 6, &opr_src);
    cpu.idtr.limit = opr_src.val;
    cpu.idtr.base = data_size == 16 ? opr_dest.val & 0xffffff : opr_dest.val;
    return len;
}