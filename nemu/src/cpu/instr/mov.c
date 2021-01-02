#include "cpu/instr.h"

static void instr_execute_2op() 
{
	operand_read(&opr_src);
	opr_dest.val = opr_src.val;
	operand_write(&opr_dest);
}

make_instr_impl_2op(mov, r, rm, b)
make_instr_impl_2op(mov, r, rm, v)
make_instr_impl_2op(mov, rm, r, b)
make_instr_impl_2op(mov, rm, r, v)
make_instr_impl_2op(mov, i, rm, b)
make_instr_impl_2op(mov, i, rm, v)
make_instr_impl_2op(mov, i, r, b)
make_instr_impl_2op(mov, i, r, v)
make_instr_impl_2op(mov, a, o, b)
make_instr_impl_2op(mov, a, o, v)
make_instr_impl_2op(mov, o, a, b)
make_instr_impl_2op(mov, o, a, v)

make_instr_func(mov_zrm82r_v) {
	int len = 1;
	OPERAND r, rm;
	r.data_size = data_size;
	rm.data_size = 8;
	len += modrm_r_rm(eip + 1, &r, &rm);
	
	operand_read(&rm);
	r.val = rm.val;
	operand_write(&r);

	print_asm_2("mov", "", len, &rm, &r);
	return len;
}

make_instr_func(mov_zrm162r_l) {
    int len = 1;
    OPERAND r, rm;
    r.data_size = 32;
    rm.data_size = 16;
    len += modrm_r_rm(eip + 1, &r, &rm);
    
    operand_read(&rm);
    r.val = rm.val;
    operand_write(&r);
    print_asm_2("mov", "", len, &rm, &r);
    return len;
}

make_instr_func(mov_srm82r_v) {
    int len = 1;
    OPERAND r, rm;
    r.data_size = data_size;
    rm.data_size = 8;
    len += modrm_r_rm(eip + 1, &r, &rm);
    
    operand_read(&rm);
    r.val = sign_ext(rm.val, 8);
    operand_write(&r);
    print_asm_2("mov", "", len, &rm, &r);
    return len;
}

make_instr_func(mov_srm162r_l) {
    int len = 1;
    OPERAND r, rm;
    r.data_size = 32;
    rm.data_size = 16;
    len += modrm_r_rm(eip + 1, &r, &rm);
    operand_read(&rm);
    r.val = sign_ext(rm.val, 16);
    operand_write(&r);
    
    print_asm_2("mov", "", len, &rm, &r);
    return len;
}

make_instr_func(mov_rm2s_w) {
    int len = 1;
    OPERAND r, rm;
    r.data_size = 16;
    rm.data_size = 16;
    len += modrm_r_rm(eip+1, &r, &rm);
    operand_read(&rm);
#ifdef IA32_SEG
    cpu.segReg[r.addr].val = rm.val;
    load_sreg(r.addr);
#endif
    return len;
}

make_instr_func(mov_c2r_l) {
    int len = 1;
    OPERAND r, rm;
    r.data_size = 32;
    rm.data_size = 32;
    len += modrm_r_rm(eip+1, &r, &rm);
#ifdef IA32_SEG
    switch (rm.addr) {
        case 0: r.val = cpu.cr0.val; break;
#ifdef IA32_PAGE
        case 3: r.val = cpu.cr3.val; break;
#endif
        default: assert(0);
    }
    operand_write(&r);
#endif
    return len;
}

make_instr_func(mov_r2c_l) {
    int len = 1;
    OPERAND r, rm;
    r.data_size = 32;
    rm.data_size = 32;
    len += modrm_r_rm(eip+1, &r, &rm);
    operand_read(&rm);
#ifdef IA32_SEG
    switch (r.addr) {
        case 0: cpu.cr0.val = rm.val; break;
#ifdef IA32_PAGE
        case 3: cpu.cr3.val = rm.val; break;
#endif
        default: assert(0);
    }
#endif
    return len;
}