#include "cpu/instr.h"
/*
Put the implementations of `call' instructions here.
*/

void push_eip()
{
    // if(data_size==16) 
    // {
    //     cpu.esp-=2;
    //     opr_dest.val=sign_ext(cpu.eip,data_size);
    // }
    // else 
    // {
    //     cpu.esp-=4;
    //     opr_dest.val=cpu.eip;
    // }
    cpu.esp-=4;
    opr_dest.val=cpu.eip;
    opr_dest.data_size=data_size;
    opr_dest.addr=cpu.esp;
    opr_dest.type=OPR_MEM;
    opr_dest.sreg=SREG_SS;
    operand_write(&opr_dest);
}


make_instr_func(call_near_indirect)
{
    int len=1;
    len+=modrm_rm(eip+1,&opr_src);
    opr_src.sreg=SREG_SS;
    operand_read(&opr_src);
    print_asm_1("call",opr_src.data_size == 8 ? "b" : (opr_src.data_size == 16 ? "w" : "l"),5,&opr_src);
    cpu.eip+=len;
    push_eip();
    // if(data_size==16)
    // {
    //     cpu.eip=opr_src.val&0x0000FFFF;
    // }else
    // {
    //     cpu.eip=opr_src.val;
    // }
    cpu.eip=opr_src.val;
    return 0;
}

make_instr_func(call_near)
{

    //len+=modrm_rm(eip+1,&opr_src);
    opr_src.type=OPR_IMM;
    opr_src.addr=cpu.eip+1;
    opr_src.data_size=32;
    opr_src.sreg=SREG_SS;
    operand_read(&opr_src);
    print_asm_1("call",opr_src.data_size == 8 ? "b" : (opr_src.data_size == 16 ? "w" : "l"),5,&opr_src);
    cpu.eip+=5;  //立即数长度
    push_eip();
    
    // if(data_size==16)
    // {
    //     cpu.eip=(cpu.eip+opr_src.val)&0x0000FFFF;
    // }
    // else
    // {
    //     cpu.eip+=opr_src.val;
    // }
     cpu.eip+=opr_src.val;
    return 0;
   
}