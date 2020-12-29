#include "memory/mmu/cache.h"
#include <time.h>
#include <stdlib.h>

static struct CacheLine {
    bool valid;
    uint16_t tag;
    uint8_t data[64];  
} cache_line[128][8];

extern uint8_t hw_mem[];
extern uint32_t hw_mem_read(paddr_t paddr, size_t len);
extern void hw_mem_write(paddr_t paddr, size_t len, uint32_t data);

// init the cache
void init_cache()
{
	// implement me in PA 3-1
    memset(cache_line, 0, sizeof(cache_line));
}

// write data to cache
void cache_write(paddr_t paddr, size_t len, uint32_t data)
{
	// implement me in PA 3-1
    uint32_t paddr_group1 = (paddr >> 6) & 0x7F, paddr_row1 = 0, cache_addr1 = paddr & 0x3F;
    uint16_t paddr_tag1 = paddr >> 13;
    uint32_t paddr_group2 = ((paddr+len-1) >> 6) & 0x7F, paddr_row2 = 0;
    uint16_t paddr_tag2 = (paddr+len-1) >> 13;
    bool hit1 = false, hit2 = true;
    for (; paddr_row1 < 8; ++paddr_row1) {
        if (cache_line[paddr_group1][paddr_row1].valid && cache_line[paddr_group1][paddr_row1].tag == paddr_tag1) {
            hit1 = true;
            break;
        }
    }
    if (cache_addr1+len-1 >= 64) {
        hit2 = false;
        for (; paddr_row2 < 8; ++paddr_row2) {
            if (cache_line[paddr_group2][paddr_row2].valid && cache_line[paddr_group2][paddr_row2].tag == paddr_tag2) {
                hit2 = true;
                break;
            }
        }
    }
    if (hit1) {
        if (cache_addr1+len-1 >= 64) {
            memcpy(cache_line[paddr_group1][paddr_row1].data+cache_addr1, &data, 64-cache_addr1);
        } else {
            memcpy(cache_line[paddr_group1][paddr_row1].data+cache_addr1, &data, len);
        }
    }
    if (hit2 && cache_addr1+len-1 >= 64) {
        uint32_t data_high = data >> ((64-cache_addr1) << 3);
        memcpy(cache_line[paddr_group2][paddr_row2].data, &data_high, cache_addr1+len-64);
    }
    hw_mem_write(paddr, len, data);
}

// read data from cache
uint32_t cache_read(paddr_t paddr, size_t len)
{
	// implement me in PA 3-1
	uint32_t paddr_group1 = (paddr >> 6) & 0x7F, paddr_row1 = 0, cache_addr1 = paddr & 0x3F;
    uint16_t paddr_tag1 = paddr >> 13;
    uint32_t paddr_group2 = ((paddr+len-1) >> 6) & 0x7F, paddr_row2 = 0;
    uint16_t paddr_tag2 = (paddr+len-1) >> 13;
    bool hit1 = false, hit2 = true;
    for (; paddr_row1 < 8; ++paddr_row1) {
        if (cache_line[paddr_group1][paddr_row1].valid && cache_line[paddr_group1][paddr_row1].tag == paddr_tag1) {
            hit1 = true;
            break;
        }
    }
    if (cache_addr1+len-1 >= 64) {
        hit2 = false;
        for (; paddr_row2 < 8; ++paddr_row2) {
            if (cache_line[paddr_group2][paddr_row2].valid && cache_line[paddr_group2][paddr_row2].tag == paddr_tag2) {
                hit2 = true;
                break;
            }
        }
    }
    uint32_t ret = 0;
    if (hit1 && hit2) {
        if (cache_addr1+len-1 >= 64) {
            uint32_t r1 = 0, r2 = 0;
            memcpy(&r1, cache_line[paddr_group1][paddr_row1].data+cache_addr1, 64-cache_addr1);
            memcpy(&r2, cache_line[paddr_group2][paddr_row2].data, cache_addr1+len-64);
            ret = r1 | (r2 << ((64-cache_addr1) << 3));
        } else {
            memcpy(&ret, cache_line[paddr_group1][paddr_row1].data+cache_addr1, len);
        }
    } else {
        ret = hw_mem_read(paddr, len);
        srand(time(0));
        paddr_row1 = rand()%8;
        cache_line[paddr_group1][paddr_row1].valid = true;
        cache_line[paddr_group1][paddr_row1].tag = paddr_tag1;
        memcpy(cache_line[paddr_group1][paddr_row1].data, hw_mem+(paddr_tag1 << 13)+(paddr_group1 << 6), 64);
        if (cache_addr1+len-1 >= 64) {
            paddr_row2 = rand()%8;
            cache_line[paddr_group2][paddr_row2].valid = true;
            cache_line[paddr_group2][paddr_row2].tag = paddr_tag2;
            memcpy(cache_line[paddr_group2][paddr_row2].data, hw_mem+(paddr_tag2 << 13)+(paddr_group2 << 6), 64);
        }
    }
	return ret;
}

