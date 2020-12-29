#include "nemu.h"
#include "cpu/cpu.h"
#include "memory/memory.h"
#include "device/mm_io.h"
#include <memory.h>
#include <stdio.h>

uint8_t hw_mem[MEM_SIZE_B];

uint32_t hw_mem_read(paddr_t paddr, size_t len)
{
	uint32_t ret = 0;
	memcpy(&ret, hw_mem + paddr, len);
	return ret;
}

void hw_mem_write(paddr_t paddr, size_t len, uint32_t data)
{
	memcpy(hw_mem + paddr, &data, len);
}

uint32_t paddr_read(paddr_t paddr, size_t len)
{
	uint32_t ret = 0;
#ifdef HAS_DEVICE_VGA
	int map_NO = is_mmio(paddr);
	if (~map_NO) return mmio_read(paddr, len, map_NO);
#endif
#ifdef CACHE_ENABLED
    ret = cache_read(paddr, len);
#else
    ret = hw_mem_read(paddr, len);
#endif
	return ret;
}

void paddr_write(paddr_t paddr, size_t len, uint32_t data)
{
#ifdef HAS_DEVICE_VGA
    int map_NO = is_mmio(paddr);
    if (~map_NO) mmio_write(paddr, len, data, map_NO);
#endif
#ifdef CACHE_ENABLED
    cache_write(paddr, len, data);
#else
    hw_mem_write(paddr, len, data);
#endif
}

uint32_t laddr_read(laddr_t laddr, size_t len)
{
#ifdef IA32_PAGE
    if (cpu.cr0.pg) {
        uint32_t offset = laddr & 0x00000FFF;
        if (offset+len-1 > 0x00000FFF) {
            int len1 = 0x00000FFF-offset+1;
            return (laddr_read(laddr+len1, len-len1) << (len1 << 3)) | laddr_read(laddr, len1);
        } else {
            laddr = page_translate(laddr);
            return paddr_read(laddr, len);
        }
    } else
#endif
	return paddr_read(laddr, len);
}

void laddr_write(laddr_t laddr, size_t len, uint32_t data)
{
#ifdef IA32_PAGE
    if (cpu.cr0.pg) {
        uint32_t offset = laddr & 0x00000FFF;
        if (offset+len-1 > 0x00000FFF) {
            int len1 = 0x00000FFF-offset+1;
            laddr_write(laddr, len1, data & (0xFFFFFFFF >> (32-(len1 << 3))));
            laddr_write(laddr+len1, len-len1, data >> (len1 << 3));
        } else {
            laddr = page_translate(laddr);
            paddr_write(laddr, len, data);
        }
    } else
#endif
	paddr_write(laddr, len, data);
}

uint32_t vaddr_read(vaddr_t vaddr, uint8_t sreg, size_t len)
{
	assert(len == 1 || len == 2 || len == 4);
#ifdef IA32_SEG
    if (cpu.cr0.pe) vaddr = segment_translate(vaddr, sreg);
#endif
	return laddr_read(vaddr, len);
}

void vaddr_write(vaddr_t vaddr, uint8_t sreg, size_t len, uint32_t data)
{
	assert(len == 1 || len == 2 || len == 4);
#ifdef IA32_SEG
    if (cpu.cr0.pe) vaddr = segment_translate(vaddr, sreg);
#endif
	laddr_write(vaddr, len, data);
}

void init_mem()
{
	// clear the memory on initiation
	memset(hw_mem, 0, MEM_SIZE_B);
#ifdef CACHE_ENABLED
    init_cache();
#endif
#ifdef TLB_ENABLED
	make_all_tlb();
	init_all_tlb();
#endif
}

uint32_t instr_fetch(vaddr_t vaddr, size_t len)
{
	assert(len == 1 || len == 2 || len == 4);
	return vaddr_read(vaddr, SREG_CS, len);
}

uint8_t *get_mem_addr()
{
	return hw_mem;
}
