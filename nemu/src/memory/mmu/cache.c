#include "memory/mmu/cache.h"

struct Cache_Line{
    uint8_t valid_byte:1;
    uint32_t sign_byte:19;
    uint8_t block[64];
};

struct Cache_Line Cache[1024];
// init the cache
void init_cache()
{
	// implement me in PA 3-1
	for (int i = 0; i < 1024; i++)
	    Cache[i].valid_byte = 0;
}

// write data to cache
void cache_write(paddr_t paddr, size_t len, uint32_t data)
{
	// implement me in PA 3-1
}

// read data from cache
uint32_t cache_read(paddr_t paddr, size_t len)
{
	// implement me in PA 3-1
	return 0;
}

