#include "cpu/instr.h"

make_instr_func(movs_v)
{
	OPERAND msi, mdi;
	msi.type = mdi.type = OPR_MEM;
	msi.sreg = mdi.sreg = SREG_DS;
	msi.data_size = mdi.data_size = data_size;
	msi.addr = cpu.gpr[REG_ESI]._32;
	mdi.addr = cpu.gpr[REG_EDI]._32;
	operand_read(&msi);
	mdi.val = msi.val;
	operand_write(&mdi);
	int incDec = 0;
	incDec = cpu.eflags.DF == 0 ? data_size / 8 : -data_size / 8;
	cpu.gpr[REG_ESI]._32 += incDec;
	cpu.gpr[REG_EDI]._32 += incDec;
	print_asm_0("movs (%%edi), (%%esi)", "", 1);
	return 1;
}

make_instr_func(movs_b)
{
	OPERAND msi, mdi;
	msi.type = mdi.type = OPR_MEM;
	msi.sreg = mdi.sreg = SREG_DS;
	msi.data_size = mdi.data_size = 8;
	msi.addr = cpu.gpr[REG_ESI]._32;
	mdi.addr = cpu.gpr[REG_EDI]._32;
	operand_read(&msi);
	mdi.val = msi.val;
	operand_write(&mdi);
	int incDec = 0;
	incDec = cpu.eflags.DF == 0 ? 1 : -1;
	cpu.gpr[REG_ESI]._32 += incDec;
	cpu.gpr[REG_EDI]._32 += incDec;
	print_asm_0("movs (%%edi), (%%esi)", "", 1);
	return 1;
}

make_instr_func(mov_c2r_l)
{
    int len=1;
    opr_src.data_size=32;
    opr_dest.data_size=32;
    len+=modrm_r_rm(eip+1,&opr_src,&opr_dest);
    opr_src.type=OPR_CREG;
    operand_read(&opr_src);
    opr_dest.val=opr_src.val;
    operand_write(&opr_dest);
    print_asm_2("mov","c2r",len,&opr_src,&opr_dest);
    return len;
}

make_instr_func(mov_r2c_l)
{
    int len=1;
    opr_src.data_size=32;
    opr_dest.data_size=32;
    len+=modrm_r_rm(eip+1,&opr_dest,&opr_src);
    
    opr_dest.type=OPR_CREG;
    operand_read(&opr_src);
    opr_dest.val=opr_src.val;
    operand_write(&opr_dest);
    print_asm_2("mov","r2c",len,&opr_src,&opr_dest);
    //for(uint8_t i=0;i<6;i++) load_sreg(i);

    return len;
}

