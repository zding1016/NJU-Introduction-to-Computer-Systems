#include "cpu/instr.h"
/*
Put the implementations of `ret' instructions here.
*/
make_instr_func(ret_near){
    opr_src.type = OPR_MEM;
    opr_src.addr = cpu.esp;
    opr_src.data_size = data_size;
    opr_src.sreg = SREG_SS;
    operand_read(&opr_src);
    print_asm_1("ret", "", 1, &opr_src);
    cpu.esp += 32 / 8; //栈顶pop
    cpu.eip = opr_src.val;
    return 0;
}

make_instr_func(ret_near_imm16){
    opr_src.type = OPR_IMM;
    opr_src.addr = cpu.eip + 1;
    opr_src.data_size = 16;
    operand_read(&opr_src); //读取imm16
    
    print_asm_1("ret", "w", 1, &opr_src);
    
    opr_dest.addr = cpu.esp;
    opr_dest.type = OPR_MEM;
    opr_dest.data_size = 32;
    opr_dest.sreg = SREG_SS;
    operand_read(&opr_dest);
    cpu.esp += 4;
    cpu.eip = opr_dest.val; //栈顶pop eip
    
    int offset = sign_ext(opr_src.val, opr_src.data_size); //计算偏移量
    
    cpu.esp += offset; //修改esp
    
    return 0;
}