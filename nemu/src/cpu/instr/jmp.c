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
    OPERAND rel;
    rel.data_size = data_size;
    len += modrm_rm(eip + 1, &rel);
    operand_read(&rel);
    print_asm_1("jmp","",len,&rel);
    if (data_size == 16)
        cpu.eip = rel.val & 0xffff;
    else
        cpu.eip = rel.val;
    return 0;
    
}


make_instr_func(jmp_short){
    OPERAND rel;
    rel.type = OPR_IMM;
    rel.sreg = SREG_CS;
    rel.data_size = 8;
    rel.addr = eip + 1;
    
    operand_read(&rel);
    
    int offset = sign_ext(rel.val, rel.data_size);
    print_asm_1("jmp","", 2,&rel);
    cpu.eip += offset;
    return 2;
}

make_instr_func(jmp_far_imm){
    OPERAND ptr16, ptr32;
	ptr16.type = ptr32.type = OPR_IMM;
	ptr16.data_size = 16;
	ptr32.data_size = 32;

	ptr16.addr = eip + 5;
	ptr32.addr = eip + 1;

	operand_read(&ptr16);
	operand_read(&ptr32);

	cpu.cs.val = ptr16.val;
	cpu.eip = ptr32.val;

	print_asm_2("ljmp", "", 7, &ptr16, &ptr32);
	load_sreg(1);
    return 0;
}