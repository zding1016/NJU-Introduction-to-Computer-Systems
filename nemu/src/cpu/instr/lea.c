#include "cpu/instr.h"
/*
Put the implementations of `lea' instructions here.
*/
make_instr_func(lea){
    opr_src.data_size = opr_dest.data_size = data_size;
    int len = 1 + modrm_r_rm(eip + 1, &opr_dest, &opr_src);
    opr_dest.val = opr_src.addr;
    print_asm_2("lea",opr_dest.data_size == 8 ? "b" :(opr_dest.data_size == 16 ? "w" : "l"), len, &opr_src, &opr_dest);
    operand_write(opr_dest);
    return len;
}