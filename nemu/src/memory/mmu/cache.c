#include "memory/mmu/cache.h"
#include "memory/memory.h"
#include <stdlib.h>


// init the cache
#ifdef CACHE_ENABLED
void init_cache()
{
    for (int i = 0; i < CACHE_SIZE; i++)
        cache[i].validbit = false;
    // implement me in PA 3-1
}

// write data to cache
void cache_write(paddr_t paddr, size_t len, uint32_t data)
{

    uint32_t block_index = paddr & (0xffffffff >> (32 - BLOCK_INDEX_LEN)); //块内地址
    uint32_t group_index = (paddr >> BLOCK_INDEX_LEN) & (0xffffffff >> (32 - GROUP_INDEX_LEN));
    //组号
    uint32_t label = (paddr >> (BLOCK_INDEX_LEN + GROUP_INDEX_LEN)); //标记
    uint32_t start = group_index * GROUP_SIZE;                       //组起始地址

    memcpy(hw_mem + paddr, &data, len);
    for (int i = start; i < start + GROUP_SIZE; i++)
    {
        //命中cache
        if (cache[i].validbit && cache[i].label == label)
        {
            if (block_index + len > BLOCK_SIZE) //数据分开写入
            {

                size_t len1 = BLOCK_SIZE - block_index;
                size_t len2 = len - len1;
                uint32_t data1 = data;               //低位
                uint32_t data2 = data >> (8 * len1); //高位
                cache_write(paddr, len1, data1);
                cache_write(paddr + len1, len2, data2);
            }
            else
            {
                memcpy(&cache[i].block[block_index], &data, len); //写cache
            }
        }
    }

    //不命中
    //不做操作
}

uint32_t cache_read(paddr_t paddr, size_t len)
{

    uint32_t block_index = paddr & (0xffffffff >> (32 - BLOCK_INDEX_LEN)); //块内地址
    if (block_index + len > BLOCK_SIZE)                                    //block越界,分开读取
    {
        size_t len1 = BLOCK_SIZE - block_index;
        size_t len2 = len - len1;
        uint32_t data1 = cache_read(paddr, len1);        //低位
        uint32_t data2 = cache_read(paddr + len1, len2); //高位
        uint32_t data = (data2 << len1 * 8) + data1;
        return data;
    }
    else
    {
        uint32_t group_index = (paddr >> BLOCK_INDEX_LEN) & (0xffffffff >> (32 - GROUP_INDEX_LEN));
        //组号
        uint32_t label = (paddr >> (BLOCK_INDEX_LEN + GROUP_INDEX_LEN)); //标记
        uint32_t start = group_index * GROUP_SIZE;                       //组起始地址
        uint32_t data = 0;
        for (int i = start; i < start + GROUP_SIZE; i++)
        {
            //命中cache
            if (cache[i].validbit && cache[i].label == label)
            {
                memcpy(&data, &cache[i].block[block_index], len); //block读取数据
                return data;
            }
        }
        //没有命中cache
        for (int i = start; i < start + GROUP_SIZE; i++)
        {
            //group有空位
            if (!cache[i].validbit)
            {
                memcpy(cache[i].block, (hw_mem + ((paddr / BLOCK_SIZE) * BLOCK_SIZE)), BLOCK_SIZE); 
                cache[i].validbit = true;
                cache[i].label = label;
                memcpy(&data, &cache[i].block[block_index], len); //重新从block读取数据
                return data;
            }
        }
        //group无空位
        int i = start + rand() % GROUP_SIZE;
        memcpy(cache[i].block, (hw_mem + ((paddr / BLOCK_SIZE) * BLOCK_SIZE)), BLOCK_SIZE);
        cache[i].validbit = true;
        cache[i].label = label;
        memcpy(&data, &cache[i].block[block_index], len); 
        return data;
    }
}
#endif
