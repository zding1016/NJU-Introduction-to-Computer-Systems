#include "cpu/intr.h"
#include "cpu/instr.h"
#include "memory/memory.h"

void raise_intr(uint8_t intr_no)
{
#ifdef IA32_INTR
	/*printf("Please implement raise_intr()");
	fflush(stdout);
	assert(0);*/
	GateDesc IDTi;
    IDTi.val[0] = laddr_read(cpu.idtr.base+(intr_no << 3), 4);
    IDTi.val[1] = laddr_read(cpu.idtr.base+(intr_no << 3)+4, 4);
    assert(IDTi.present == 1);
    SegDesc GDTi;
    GDTi.val[0] = laddr_read(cpu.gdtr.base+((IDTi.selector >> 3) << 3), 4);
    GDTi.val[1] = laddr_read(cpu.gdtr.base+((IDTi.selector >> 3) << 3)+4, 4);
    assert(GDTi.present == 1);
    if (cpu.cs.rpl < GDTi.privilege_level) {
        assert(0);
    } else if (cpu.cs.rpl > IDTi.privilege_level) {
        assert(0);
    } else {
        if (cpu.cs.rpl > GDTi.privilege_level) {
            assert(0);
        }
        push_(cpu.eflags.val, 32);
        push_(cpu.cs.val, 32);
        push_(cpu.eip, 32);
        if (IDTi.type == 14) cpu.eflags.IF = 0;
        cpu.cs.val = IDTi.selector;
        load_sreg(1);
        cpu.eip = (IDTi.offset_31_16 << 16)+IDTi.offset_15_0;
    }
#endif
}

void raise_sw_intr(uint8_t intr_no)
{
	// return address is the next instruction
	cpu.eip += 2;
	//printf("intr cpu.eip : %x\n", cpu.eip);
	raise_intr(intr_no);
	//printf("intr cpu.eip : %x\n", cpu.eip);
}
