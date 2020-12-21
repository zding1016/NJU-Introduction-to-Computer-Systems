#include "cpu/cpu.h"
#include "memory/memory.h"

// return the linear address from the virtual address and segment selector
uint32_t segment_translate(uint32_t offset, uint8_t sreg)
{
	/* TODO: perform segment translation from virtual address to linear address
	 * by reading the invisible part of the segment register 'sreg'
	 */
	return offset + cpu.segReg[sreg].base;
}

// load the invisible part of a segment register
void load_sreg(uint8_t sreg)
{
	/* TODO: load the invisibile part of the segment register 'sreg' by reading the GDT.
	 * The visible part of 'sreg' should be assigned by mov or ljmp already.
	 */
    assert (sreg < 6);
    SegDesc temp;
    memcpy(&temp, hw_mem + cpu.gdtr.base + cpu.segReg[sreg].index * 8, sizeof(temp));
    assert (temp.present == 1);
    assert (temp.granularity == 1);
    uint32_t base, limit, type, privilege_level, soft_use;
    
    base = (temp.base_31_24 << 24) + (temp.base_23_16 << 16) + temp.base_15_0;
    limit = (temp.limit_19_16 << 16) + temp.limit_15_0;
    type = temp.type;
    privilege_level = temp.privilege_level;
    soft_use = temp.soft_use;
    
    assert (base == 0);
    cpu.segReg[sreg].base = base;
    cpu.segReg[sreg].limit = limit;
    cpu.segReg[sreg].type = type;
    cpu.segReg[sreg].privilege_level = privilege_level;
    cpu.segReg[sreg].soft_use = soft_use;

}
