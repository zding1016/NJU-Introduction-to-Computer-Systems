#include "memory/mmu/cache.h"
#include "memory/memory.h"
#include <stdlib.h>
#define BLOCK_SIZE 64
#define CACHE_SIZE 1024
#define GROUP_SIZE 8
#define BLOCK_INDEX_LEN 6
#define GROUP_INDEX_LEN 7
#define LABEL_INDEX_LEN 19
struct Cache_Line{
    bool validbit; //有效位 1位
    uint32_t label : 19; //标记位 32 - 6 - 7 = 19位 
    uint8_t block[64]; // 2^6 位
};

struct Cache_Line cache[1024]; //共有1024行
//但是我们要实现8行一组，所以共有2^7组，组号的位数为7
// init the cache
void init_cache()
{
	// implement me in PA 3-1
	for(int i = 0; i < CACHE_SIZE; i++) cache[i].validbit = false;
}

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
        /*
        uint32_t true_data;
        memcpy(&true_data,hw_mem+paddr,len);
        printf("data1:0x%08x data2:0x%08x data:0x%08x true:0x%08x\n",data1,data2,data,true_data);*/
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
                memcpy(cache[i].block, (hw_mem + ((paddr / BLOCK_SIZE) * BLOCK_SIZE)), BLOCK_SIZE); //将内存拷入cache
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
        memcpy(&data, &cache[i].block[block_index], len); //重新从block读取数据
        return data;
    }
}

