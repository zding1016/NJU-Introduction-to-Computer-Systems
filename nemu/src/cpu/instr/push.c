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

make_instr_func(pusha)
{
    uint32_t temp=cpu.esp;
    print_asm_0("push","a",1);
    for(int i=0;i<8;i++){
        cpu.esp-=4;
        opr_dest.data_size=data_size;
        opr_dest.val=i==4?temp:cpu.gpr[i].val;
        opr_dest.addr=cpu.esp;
        opr_dest.type=OPR_MEM;
        opr_dest.sreg=SREG_SS;
        operand_write(&opr_dest);
    }
    return 1;
}