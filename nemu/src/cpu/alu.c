#include "cpu/cpu.h"
#include <stdarg.h>

uint32_t resize(uint32_t src, size_t data_size)
{
    return src & (0xFFFFFFFF >> (32-data_size)); 
}

void multiresize(uint32_t cnt, uint32_t data_size, uint32_t *src, ...)
{
    va_list ap;
    va_start(ap, src);
    *src = resize(*src, data_size);
    for (int i = 1; i < cnt; ++i) {
        src = va_arg(ap, uint32_t*);
        *src = resize(*src, data_size);
    }
    va_end(ap);
}

bool high_bit(uint32_t src, size_t data_size)
{
    return resize(src, data_size) >> (data_size-1);
}

bool low_bit(uint32_t src) {
    return src & 1;
}

void set_ZF(uint32_t res, size_t data_size)
{
    cpu.eflags.ZF = resize(res, data_size) == 0;
}

void set_SF(uint32_t res, size_t data_size)
{
    cpu.eflags.SF = high_bit(res, data_size);
}

void set_OF(uint32_t src, uint32_t dest, uint32_t res, size_t data_size)
{
    cpu.eflags.OF = (high_bit(src, data_size) == high_bit(dest, data_size)) && (high_bit(src, data_size) != high_bit(res, data_size));
}

void set_CF(uint32_t src, uint32_t dest, uint32_t res, uint32_t cn_1, uint32_t cin, size_t data_size)
{
    cpu.eflags.CF = ((src+cn_1 < src) || (res < src+cn_1)) ^ cin;
}

void set_PF(uint32_t res)
{
    int cnt = 0;
    for (int i = 1; i <= 8; ++i) {
        cnt += res & 1;
        res >>= 1;
    }
    cpu.eflags.PF = !(cnt & 1);
}

uint32_t alu_add(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_add(src, dest, data_size);
#else
	uint32_t res = src+dest;
	multiresize(3, data_size, &src, &dest, &res);
	set_ZF(res, data_size);
	set_SF(res, data_size);
	set_PF(res);
	set_OF(src, dest, res, data_size);
	set_CF(src, dest, res, 0, 0, data_size);
	return res;
#endif
}

uint32_t alu_adc(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_adc(src, dest, data_size);
#else
	uint32_t res = src+dest+cpu.eflags.CF;
	multiresize(3, data_size, &src, &dest, &res);
	set_ZF(res, data_size);
	set_SF(res, data_size);
	set_PF(res);
	set_OF(src, dest, res, data_size);
	set_CF(src, dest, res, cpu.eflags.CF, 0, data_size);
	return res;
#endif
}

uint32_t alu_sub(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_sub(src, dest, data_size);
#else
    src = ~src;
    uint32_t res = src+dest+1;
    multiresize(3, data_size, &src, &dest, &res);
	set_ZF(res, data_size);
	set_SF(res, data_size);
	set_PF(res);
	set_OF(src, dest, res, data_size);
	set_CF(src, dest, res, 1, 1, data_size);
	return res;
#endif
}

uint32_t alu_sbb(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_sbb(src, dest, data_size);
#else
    src = ~src;
    uint32_t res = src+dest+(!cpu.eflags.CF);
    multiresize(3, data_size, &src, &dest, &res);
	set_ZF(res, data_size);
	set_SF(res, data_size);
	set_PF(res);
	set_OF(src, dest, res, data_size);
	set_CF(src, dest, res, !cpu.eflags.CF, 1, data_size);
	return res;
#endif
}

uint64_t alu_mul(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_mul(src, dest, data_size);
#else
    uint64_t res = (uint64_t)src*(uint64_t)dest;
    cpu.eflags.CF = cpu.eflags.OF = ((res >> data_size) & (0xFFFFFFFF >> (32-data_size))) > 0;
    return res;
#endif
}

int64_t alu_imul(int32_t src, int32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_imul(src, dest, data_size);
#else
    int64_t res = (int64_t)src*(int64_t)dest;
    cpu.eflags.CF = cpu.eflags.OF = ((res >> data_size) & (0xFFFFFFFF >> (32-data_size))) > 0;
    return res;
#endif
}

// need to implement alu_mod before testing
uint32_t alu_div(uint64_t src, uint64_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_div(src, dest, data_size);
#else
    assert(src);
    uint32_t res = dest/src;
    return resize(res, data_size);
#endif
}

// need to implement alu_imod before testing
int32_t alu_idiv(int64_t src, int64_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_idiv(src, dest, data_size);
#else
    assert(src);
    uint32_t res = dest/src;
    return resize(res, data_size);
#endif
}

uint32_t alu_mod(uint64_t src, uint64_t dest)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_mod(src, dest);
#else
    uint32_t res = dest%src;
    return res;
#endif
}

int32_t alu_imod(int64_t src, int64_t dest)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_imod(src, dest);
#else
    uint32_t res = dest%src;
    return res;
#endif
}

uint32_t alu_and(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_and(src, dest, data_size);
#else
    uint32_t res = src & dest;
    multiresize(1, data_size, &res);
    set_ZF(res, data_size);
    set_PF(res);
    set_SF(res, data_size);
    cpu.eflags.CF = cpu.eflags.OF = 0;
    return res;
#endif
}

uint32_t alu_xor(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_xor(src, dest, data_size);
#else
    uint32_t res = src ^ dest;
    multiresize(1, data_size, &res);
    set_ZF(res, data_size);
    set_PF(res);
    set_SF(res, data_size);
    cpu.eflags.CF = cpu.eflags.OF = 0;
    return res;
#endif
}

uint32_t alu_or(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_or(src, dest, data_size);
#else
    uint32_t res = src | dest;
    multiresize(1, data_size, &res);
    set_ZF(res, data_size);
    set_PF(res);
    set_SF(res, data_size);
    cpu.eflags.CF = cpu.eflags.OF = 0;
    return res;
#endif
}

uint32_t alu_shl(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_shl(src, dest, data_size);
#else
    multiresize(2, data_size, &src, &dest);
    uint32_t res = dest << src;
    multiresize(1, data_size, &res);
    set_ZF(res, data_size);
    set_PF(res);
    set_SF(res, data_size);
    cpu.eflags.CF = src ? high_bit(dest << (src-1), data_size) : cpu.eflags.CF;
    cpu.eflags.OF = src == 1 ? high_bit(res, data_size) : cpu.eflags.OF;
    return res;
#endif
}

uint32_t alu_shr(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_shr(src, dest, data_size);
#else
    multiresize(2, data_size, &src, &dest);
    uint32_t res = dest >> src;
    multiresize(1, data_size, &res);
    set_ZF(res, data_size);
    set_PF(res);
    set_SF(res, data_size);
    cpu.eflags.CF = src ? low_bit(dest >> (src-1)) : cpu.eflags.CF;
    cpu.eflags.OF = src == 1 ? high_bit(res, data_size) : cpu.eflags.OF;
    return res;
#endif
}

uint32_t alu_sar(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_sar(src, dest, data_size);
#else
    multiresize(2, data_size, &src, &dest);
    uint32_t res = dest >> src;
    res = high_bit(dest, data_size) ? ((0xFFFFFFFF << (data_size-src)) | res) : res;
    multiresize(1, data_size, &res);
    set_ZF(res, data_size);
    set_PF(res);
    set_SF(res, data_size);
    cpu.eflags.CF = src ? low_bit(dest >> (src-1)) : cpu.eflags.CF;
    cpu.eflags.OF = src == 1 ? 0 : cpu.eflags.OF;
    return res;
#endif
}

uint32_t alu_sal(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_sal(src, dest, data_size);
#else
    multiresize(2, data_size, &src, &dest);
    uint32_t res = dest << src;
    multiresize(1, data_size, &res);
    set_ZF(res, data_size);
    set_PF(res);
    set_SF(res, data_size);
    cpu.eflags.CF = src ? high_bit((dest << (src-1)), data_size) : cpu.eflags.CF;
    cpu.eflags.OF = src == 1 ? high_bit(res, data_size) : cpu.eflags.OF;
    return res;
#endif
}
