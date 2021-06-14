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

make_instr_func(jmp_far_imm) {
    OPERAND imm1, imm2;
    imm1.type = OPR_IMM;
    imm1.sreg = SREG_CS;
    imm1.data_size = 16;
    imm1.addr = eip+data_size/8+1;
    imm2.type = OPR_IMM;
    imm2.sreg = SREG_CS;
    imm2.data_size = data_size;
    imm2.addr = eip+1;
    operand_read(&imm1);
    operand_read(&imm2);
#ifdef IA32_SEG
    cpu.cs.val = imm1.val;
    load_sreg(1);
#endif
    cpu.eip = imm2.val & (0xFFFFFFFF >> (32-data_size));
    return 0;
}

make_instr_func(jmp_short) {
    OPERAND rel;
    rel.type = OPR_IMM;
    rel.sreg = SREG_CS;
    rel.data_size = 8;
    rel.addr = eip + 1;

    operand_read(&rel);

    int offset = sign_ext(rel.val, 8);
    // thank Ting Xu from CS'17 for finding this bug
    print_asm_1("jmp", "", 2, &rel);

    cpu.eip += offset;
    return 2;
}

make_instr_func(jmp_near_indirect) {
    OPERAND rm;
    rm.data_size = data_size;
    modrm_rm(eip+1, &rm);
    operand_read(&rm);
    cpu.eip = rm.val & (0xFFFFFFFF >> (32-data_size));
    return 0;
}