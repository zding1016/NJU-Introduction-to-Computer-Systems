#include "cpu/cpu.h"
#include "memory/memory.h"

// translate from linear address to physical address
paddr_t page_translate(laddr_t laddr)
{
#ifndef TLB_ENABLED
#ifdef IA32_PAGE
	PDE dirEntry;
	dirEntry.val = paddr_read((cpu.cr3.pdbr << 12) | (((laddr >> 22) & 0x000003FF) << 2), 4);
	assert(dirEntry.present == 1);
	PTE tableEntry;
	tableEntry.val = paddr_read((dirEntry.page_frame << 12) | (((laddr >> 12) & 0x000003FF) << 2), 4);
	assert(tableEntry.present == 1);
	return (tableEntry.page_frame << 12) | (laddr & 0x00000FFF);
#else
    return 0;
#endif
#else
	return tlb_read(laddr) | (laddr & PAGE_MASK);
#endif
}
