#ifndef __INSTR_PUSH_H__
#define __INSTR_PUSH_H__
/*
Put the declarations of `push' instructions here.
*/
void push_help(uint32_t val, int datasize);
make_instr_func(push_r_v);
make_instr_func(push_rm_v);
make_instr_func(push_i_b);
make_instr_func(push_i_v);
make_instr_func(push_r_w);
make_instr_func(pusha);
#endif
