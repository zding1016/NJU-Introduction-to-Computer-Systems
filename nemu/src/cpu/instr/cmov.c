#include "cpu/instr.h"

static void instr_execute_2op_cc_pass()
{
	operand_read(&opr_src);
	opr_dest.val = opr_src.val;
	operand_write(&opr_dest);
}

static void instr_execute_2op_cc_fail() { /*Do nothing*/}

make_instr_impl_2op_cc(cmov, rm, r, v, a)
make_instr_impl_2op_cc(cmov, rm, r, v, ae)
make_instr_impl_2op_cc(cmov, rm, r, v, b)
make_instr_impl_2op_cc(cmov, rm, r, v, be)
make_instr_impl_2op_cc(cmov, rm, r, v, g)
make_instr_impl_2op_cc(cmov, rm, r, v, ge)
make_instr_impl_2op_cc(cmov, rm, r, v, l)
make_instr_impl_2op_cc(cmov, rm, r, v, le)
make_instr_impl_2op_cc(cmov, rm, r, v, ne)
make_instr_impl_2op_cc(cmov, rm, r, v, no)
make_instr_impl_2op_cc(cmov, rm, r, v, np)
make_instr_impl_2op_cc(cmov, rm, r, v, ns)
make_instr_impl_2op_cc(cmov, rm, r, v, o)
make_instr_impl_2op_cc(cmov, rm, r, v, p)
make_instr_impl_2op_cc(cmov, rm, r, v, s)
make_instr_impl_2op_cc(cmov, rm, r, v, e)


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