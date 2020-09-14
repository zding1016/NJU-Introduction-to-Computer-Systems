#include "cpu/cpu.h"

uint32_t get_mask(size_t data_size){
    return (0xffffffff >> (32-data_size));
}

//设置标志位
void set_ZF(uint32_t result, size_t data_size){ 
    result = result & get_mask(data_size);
    uint32_t ans = (result == 0);
    cpu.eflags.ZF = ans;
}

void set_PF(uint32_t result){
    result = result & 0xff;
    uint32_t ans = 0;
    for (int i = 0; i < 8; i++){
        ans += result & 1;
        result = result >> 1;
    }
    cpu.eflags.PF = (ans % 2 == 0);
}

void set_SF(uint32_t result, size_t data_size){
    uint32_t ans = (result >> (data_size - 1)) & 1;
    cpu.eflags.SF = ans;
}

void set_CF_add(uint32_t src, uint32_t dest, uint32_t result){
    cpu.eflags.CF = (result < src) || (result < dest);
}

void set_OF_ad(uint32_t src, uint32_t dest, uint32_t result, size_t data_size){
    src = (src >> (data_size - 1)) & 1;
    dest = (dest >> (data_size - 1)) & 1;
    result = (result >> (data_size - 1)) & 1;
    cpu.eflags.OF = (src && dest && !result) || (!src && !dest && result);
}

void set_OF_sb(uint32_t src, uint32_t dest, uint32_t result, size_t data_size){
    src = (src >> (data_size - 1)) & 1;
    dest = (dest >> (data_size - 1)) & 1;
    result = (result >> (data_size - 1)) & 1;
    cpu.eflags.OF = (src && !dest && result) || (!src && dest && !result);
}


void set_CF_sub(uint32_t src, uint32_t dest, size_t data_size){
    src = sign_ext(src & get_mask(data_size), data_size);
    dest = sign_ext(dest & get_mask(data_size), data_size);
    cpu.eflags.CF = (dest < src);
}

void set_CF_sbb(uint32_t src, uint32_t dest, size_t data_size){
    src = sign_ext(src & get_mask(data_size), data_size);
    dest = sign_ext(dest & get_mask(data_size), data_size);
    if (cpu.eflags.CF == 1)
        cpu.eflags.CF = (dest <= src);
    else
        cpu.eflags.CF = (dest < src);
}

uint32_t alu_add(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_add(src, dest, data_size);
#else
	/*printf("\e[0;31mPlease implement me at alu.c\e[0m\n");
	fflush(stdout);
	assert(0);
	return 0;*/
	src = src & get_mask(data_size);
	dest = dest & get_mask(data_size);
	uint32_t result = src + dest;
	result = result & get_mask(data_size);
	set_ZF(result,data_size);
	set_PF(result);
	set_SF(result,data_size);
	set_OF_ad(src,dest,result,data_size);
	set_CF_add(src,dest,result);
	return result;
#endif
}

uint32_t alu_adc(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_adc(src, dest, data_size);
#else
	/*printf("\e[0;31mPlease implement me at alu.c\e[0m\n");
	fflush(stdout);
	assert(0);
	return 0;*/
	uint32_t CF_before = cpu.eflags.CF;
	uint32_t result = alu_add(src, dest, data_size);
	uint32_t CF_mid = cpu.eflags.CF;
	if (!CF_before) return result;
	result = alu_add(result, CF_before, data_size);
	if (CF_mid) cpu.eflags.CF = 1;
	set_OF_ad(src,dest,result,data_size);
	return result;
#endif
}

uint32_t alu_sub(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_sub(src, dest, data_size);
#else
	/*printf("\e[0;31mPlease implement me at alu.c\e[0m\n");
	fflush(stdout);
	assert(0);
	return 0;*/
	uint32_t result = dest - src;
	result = result & get_mask(data_size);
	set_ZF(result,data_size);
	set_PF(result);
	set_SF(result,data_size);
	set_CF_sub(src, dest, data_size);
    set_OF_sb(src,dest,result,data_size);
	return result;
#endif
}

uint32_t alu_sbb(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_sbb(src, dest, data_size);
#else
	/*printf("\e[0;31mPlease implement me at alu.c\e[0m\n");
	fflush(stdout);
	assert(0);
	return 0;*/
	uint32_t result = dest - src - cpu.eflags.CF;
	result = result & get_mask(data_size);
	set_ZF(result,data_size);
	set_PF(result);
	set_SF(result,data_size);
	set_OF_sb(src,dest,result,data_size);
	set_CF_sbb(src, dest, data_size);
	return result;
#endif
}

uint64_t alu_mul(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_mul(src, dest, data_size);
#else
	/*printf("\e[0;31mPlease implement me at alu.c\e[0m\n");
	fflush(stdout);
	assert(0);
	return 0;*/
	uint64_t result = (uint64_t) src * dest;
	result =result & get_mask(data_size);
	cpu.eflags.OF = cpu.eflags.CF = (result >> data_size) & 1;
	return result;
#endif
}

int64_t alu_imul(int32_t src, int32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_imul(src, dest, data_size);
#else
	printf("\e[0;31mPlease implement me at alu.c\e[0m\n");
	fflush(stdout);
	assert(0);
	return 0;
#endif
}

// need to implement alu_mod before testing
uint32_t alu_div(uint64_t src, uint64_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_div(src, dest, data_size);
#else
	printf("\e[0;31mPlease implement me at alu.c\e[0m\n");
	fflush(stdout);
	assert(0);
	return 0;
#endif
}

// need to implement alu_imod before testing
int32_t alu_idiv(int64_t src, int64_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_idiv(src, dest, data_size);
#else
	printf("\e[0;31mPlease implement me at alu.c\e[0m\n");
	fflush(stdout);
	assert(0);
	return 0;
#endif
}

uint32_t alu_mod(uint64_t src, uint64_t dest)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_mod(src, dest);
#else
	printf("\e[0;31mPlease implement me at alu.c\e[0m\n");
	fflush(stdout);
	assert(0);
	return 0;
#endif
}

int32_t alu_imod(int64_t src, int64_t dest)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_imod(src, dest);
#else
	printf("\e[0;31mPlease implement me at alu.c\e[0m\n");
	fflush(stdout);
	assert(0);
	return 0;
#endif
}

uint32_t alu_and(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_and(src, dest, data_size);
#else
	/*printf("\e[0;31mPlease implement me at alu.c\e[0m\n");
	fflush(stdout);
	assert(0);
	return 0;*/
	uint32_t result = src & dest;
	cpu.eflags.OF = cpu.eflags.CF = 0;
	result = result & get_mask(data_size);
	set_ZF(result,data_size);
	set_PF(result);
	set_SF(result,data_size);
	return result;
#endif
}

uint32_t alu_xor(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_xor(src, dest, data_size);
#else
	/*printf("\e[0;31mPlease implement me at alu.c\e[0m\n");
	fflush(stdout);
	assert(0);
	return 0;*/
	uint32_t result = src ^ dest;
	cpu.eflags.OF = cpu.eflags.CF = 0;
	result = result & get_mask(data_size);
	set_ZF(result,data_size);
	set_PF(result);
	set_SF(result,data_size);
	return result;
#endif
}

uint32_t alu_or(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_or(src, dest, data_size);
#else
	/*printf("\e[0;31mPlease implement me at alu.c\e[0m\n");
	fflush(stdout);
	assert(0);
	return 0;*/
	uint32_t result = src | dest;
	cpu.eflags.OF = cpu.eflags.CF = 0;
	result = result & get_mask(data_size);
	set_ZF(result,data_size);
	set_PF(result);
	set_SF(result,data_size);
	return result;
#endif
}

uint32_t alu_shl(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_shl(src, dest, data_size);
#else
	/*printf("\e[0;31mPlease implement me at alu.c\e[0m\n");
	fflush(stdout);
	assert(0);
	return 0;*/
	return alu_sal(src,dest,data_size);
#endif
}

uint32_t alu_shr(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_shr(src, dest, data_size);
#else
	/*printf("\e[0;31mPlease implement me at alu.c\e[0m\n");
	fflush(stdout);
	assert(0);
	return 0;*/
	cpu.eflags.CF = 0;
	if (src <= data_size)
	    cpu.eflags.CF = (dest >> (src - 1)) & 1;
	uint32_t result = dest;
	result = result & get_mask(data_size);
	result = result >> src;
	set_ZF(result,data_size);
	set_PF(result);
	set_SF(result,data_size);
	return result;
#endif
}

uint32_t alu_sar(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_sar(src, dest, data_size);
#else
	/*printf("\e[0;31mPlease implement me at alu.c\e[0m\n");
	fflush(stdout);
	assert(0);
	return 0;*/
	cpu.eflags.CF = 0;
	if (src <= data_size)
	    cpu.eflags.CF = (dest >> (src - 1)) & 1;
	uint32_t result = dest;
	result = result >> src;
	result = result & get_mask(data_size);
	set_ZF(result,data_size);
	set_PF(result);
	set_SF(result,data_size);
	return result;
#endif
}

uint32_t alu_sal(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_sal(src, dest, data_size);
#else
	/*printf("\e[0;31mPlease implement me at alu.c\e[0m\n");
	fflush(stdout);
	assert(0);
	return 0;*/
	uint32_t result = dest << src;
	result = result & get_mask(data_size); 
	cpu.eflags.CF = 0;
	if (src <= data_size) 
	    cpu.eflags.CF = (dest >> (data_size - src)) & 1;
	set_ZF(result,data_size);
	set_PF(result);
	set_SF(result,data_size);
	return result;
#endif
}
