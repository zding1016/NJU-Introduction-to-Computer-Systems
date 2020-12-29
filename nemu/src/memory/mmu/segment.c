#include "cpu/cpu.h"
#include "memory/memory.h"

// return the linear address from the virtual address and segment selector
uint32_t segment_translate(uint32_t offset, uint8_t sreg)
{
	/* TODO: perform segment translation from virtual address to linear address
	 * by reading the invisible part of the segment register 'sreg'
	 */
#ifdef IA32_SEG
	return cpu.segReg[sreg].base+offset;
#else
    return 0;
#endif
}

// load the invisible part of a segment register
void load_sreg(uint8_t sreg)
{
	/* TODO: load the invisibile part of the segment register 'sreg' by reading the GDT.
	 * The visible part of 'sreg' should be assigned by mov or ljmp already.
	 */
#ifdef IA32_SEG
	SegDesc desc;
	desc.val[0] = laddr_read(cpu.gdtr.base+(cpu.segReg[sreg].index << 3), 4);
	desc.val[1] = laddr_read(cpu.gdtr.base+(cpu.segReg[sreg].index << 3)+4, 4);
    cpu.segReg[sreg].base = (desc.base_31_24 << 24)+(desc.base_23_16 << 16)+desc.base_15_0;
    cpu.segReg[sreg].limit = (desc.limit_19_16 << 16)+desc.limit_15_0;
    cpu.segReg[sreg].privilege_level = desc.privilege_level;
    cpu.segReg[sreg].soft_use = desc.soft_use;
    assert(cpu.segReg[sreg].base == 0);
    assert(cpu.segReg[sreg].limit == 0xFFFFF);
    assert(desc.granularity == 1);
#endif
}
