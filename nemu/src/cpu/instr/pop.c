#include "cpu/instr.h"
/*
Put the implementations of `pop' instructions here.
*/
static void instr_execute_1op(){
    opr_dest.addr = opr_src.addr;
    opr_dest.type = opr_src.type;
    opr_dest.data_size = opr_src.data_size;
    opr_src.addr = cpu.esp;
    opr_src.type = OPR_MEM;
    operand_read(&opr_src);
    opr_dest.val = opr_src.val;
    if (data_size == 16)
        cpu.esp += 2;
    else
        cpu.esp += 4;         //出栈,修改指针
    operand_write(&opr_dest); //写入dest
}

make_instr_impl_1op(pop, r, v)
make_instr_impl_1op(pop, rm, v)
make_instr_impl_1op(pop, i, b)
make_instr_impl_1op(pop, i, v)

make_instr_func(popa)
{
    print_asm_0("pop","a",1);
    for(int i=7;i>=0;i--)
    {
        opr_src.addr=cpu.esp;
        opr_src.type=OPR_MEM;
        opr_src.sreg=SREG_SS;
        opr_src.data_size=data_size;
        operand_read(&opr_src);
        
        cpu.esp+=data_size/8;
        if(i==4) continue;
        else cpu.gpr[i].val=opr_src.val;
    }
    return 1;
}