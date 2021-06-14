#include "cpu/cpu.h"
#include "memory/memory.h"

// translate from linear address to physical address
paddr_t page_translate(laddr_t laddr)
{
#ifndef TLB_ENABLED
#ifdef IA32_PAGE
	PDE pde;
	pde.val = paddr_read((cpu.cr3.pdbr << 12) | (((laddr >> 22) & 0x000003FF) << 2), 4);
	assert(pde.present == 1);
	PTE pte;
	pte.val = paddr_read((pde.page_frame << 12) | (((laddr >> 12) & 0x000003FF) << 2), 4);
	assert(pte.present == 1);
	return (pte.page_frame << 12) | (laddr & 0x00000FFF);
#else
    return 0;
#endif
#else
	return tlb_read(laddr) | (laddr & PAGE_MASK);
#endif
}
