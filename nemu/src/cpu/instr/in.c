#include "cpu/instr.h"
#include "device/port_io.h"
#define AL cpu.gpr[0]._8[0]
#define AX cpu.gpr[0]._16
#define DX cpu.gpr[2]._16
/*
Put the implementations of `in' instructions here.
*/
make_instr_func(in_b) {
	print_asm_0("in", "b", 1);
	AL = (uint8_t)pio_read(DX, 1);
	return 1;
}

make_instr_func(in_v) {
	print_asm_0("in", (data_size == 16) ? "w" : "l", 1);
	if (data_size == 16)
        AX = (uint16_t)pio_read(DX, 2);
    else
        cpu.eax = pio_read(DX, 4);
    return 1;
}