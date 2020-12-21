#include "cpu/instr.h"

make_instr_func(jmp_near)
{
        OPERAND rel;
        rel.type = OPR_IMM;
        rel.sreg = SREG_CS;
        rel.data_size = data_size;
        rel.addr = eip + 1;

        operand_read(&rel);

        int offset = sign_ext(rel.val, data_size);
        // thank Ting Xu from CS'17 for finding this bug
        print_asm_1("jmp", "", 1 + data_size / 8, &rel);

        cpu.eip += offset;

        return 1 + data_size / 8;
}


make_instr_func(jmp_near_indirect){
    int len = 1;
    len += modrm_rm(eip + 1, &opr_src);
    opr_src.sreg = SREG_SS;
    operand_read(&opr_src);
    print_asm_1("jmp","",len,&opr_src);
    cpu.eip = opr_src.val;
    return 0;
    
}


make_instr_func(jmp_short){
    opr_src.type = OPR_IMM;
    opr_src.sreg = SREG_CS;
    opr_src.data_size = 8;
    opr_src.addr = eip + 1;
    
    operand_read(&opr_src);
    
    int offset = sign_ext(opr_src.val, opr_src.data_size);
    print_asm_1("jmp", "", 2, &opr_src);
    cpu.eip += offset;
    return 2;
}

make_instr_func(jmp_far_imm){
    opr_src.data_size = 16;
    opr_src.type = OPR_IMM;
    opr_src.sreg = SREG_CS;
    opr_src.addr = eip + 5;
    
    opr_dest.data_size = 32;
    opr_dest.type = OPR_IMM;
    opr_dest.sreg = SREG_CS;
    opr_dest.addr = eip + 1;
    
    operand_read(&opr_src);
    operand_read(&opr_dest);
    
    print_asm_2("ljmp", "", 7, &opr_src, &opr_dest);
    cpu.cs.val = opr_src.val;
    load_sreg(1);
    cpu.eip = opr_dest.val;
    return 0;
}