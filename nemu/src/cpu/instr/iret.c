#include "cpu/instr.h"
/*
Put the implementations of `iret' instructions here.
*/
make_instr_func(iret) {
#ifdef IA32_INTR
    int cpl = cpu.cs.rpl;
    cpu.eip = pop_(32);
    cpu.cs.val = pop_(32);
    load_sreg(1);
    cpu.eflags.val = pop_(32);
    if (cpl != cpu.cs.rpl) {
        assert(0);
    }
#endif
    return 0;
}