#include "cpu/instr.h"
/*
Put the implementations of `iret' instructions here.
*/

uint32_t POP(uint8_t DataSize)
{
    opr_src.data_size=DataSize;
    opr_src.sreg=SREG_SS;
    opr_src.addr=cpu.esp;
    opr_src.type=OPR_MEM;
    operand_read(&opr_src);
    cpu.esp+=DataSize/8;
    //fprintf(stderr,"\n%08x,%08x\n",cpu.esp,opr_src.val);
    return opr_src.val;
}

make_instr_func(iret)
{
    print_asm_0("iret","",1);
    cpu.eip=POP(32);
    cpu.cs.val=(uint16_t)POP(32);
    cpu.eflags.val=POP(32);
    return 0;
}
