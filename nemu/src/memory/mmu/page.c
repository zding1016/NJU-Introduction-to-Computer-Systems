#include "cpu/cpu.h"
#include "memory/memory.h"

// translate from linear address to physical address
paddr_t page_translate(laddr_t laddr)
{
#ifndef TLB_ENABLED
    uint32_t dir=laddr>>22;
	uint32_t page=(laddr>>12)&(0xffffffff>>22);
	uint32_t offset=laddr&(0xffffffff>>20);
	//fprintf(stderr,"%x %x %x %x",laddr,dir,page,offset);
	//page_directory
    uint32_t page_directory_index=cpu.cr3.page_directory_base<<12;
    PDE page_directory;
    memcpy(&page_directory,hw_mem+page_directory_index+dir*4,sizeof(page_directory));
    
    assert(page_directory.present==1);
    //page_table
    uint32_t page_table_index=page_directory.page_frame<<12;
    PTE page_table;
    //fprintf(stderr,"pageTable_addr is %x\n",page_table_index);
	//fprintf(stderr,"addr is %x\n",page_table_index+page*4);
    memcpy(&page_table,hw_mem+page_table_index+page*4,sizeof(page_table));
     
    assert(page_table.present==1);
    //page
   
    return (page_table.page_frame<<12)+offset;
	
#else
	return tlb_read(laddr) | (laddr & PAGE_MASK);
#endif
}
