#include "memory/mmu/cache.h"
#include "memory/memory.h"
#include <stdlib.h>
struct Cache_Line{
    uint8_t valid_byte:1; //有效位 1位
    uint32_t sign_byte:19; //标记位 32 - 6 - 7 = 19位 
    uint8_t block[64]; // 2^6 位
};

struct Cache_Line Cache[1024]; //共有1024行
//但是我们要实现8行一组，所以共有2^7组，组号的位数为7
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
	uint32_t sign_byte = (paddr >> 13) & 0x7ffff;
	uint32_t index_for_group = (paddr >> 6) & 0x7f;
	uint32_t index_in_group = paddr & 0x3f;
	bool whether_hit = false;
	uint32_t index_empty_in_group = -1;
	
	memcpy(hw_mem + paddr, &data, len);
	for (int i = 0; i< 8; i++){
	    uint32_t index_line = index_for_group *8 + i;
	    if (Cache[index_line].valid_byte == 1){
	        if (Cache[index_line].sign_byte == sign_byte) {
	            whether_hit = true;
	            for (int j = 0; j< len; j++){
	                if (index_in_group +j >= 64) {
	                    cache_write(paddr + j, len-j, data);
	                    return;
	                }
	                Cache[index_line].block[index_in_group +j] =data &0xff;
	                data = data>>8;
	            }
	            return;
	        }
	    }else {
	        index_empty_in_group = index_line;
	    }
	}
	if (!whether_hit){
	    uint32_t start_addr = paddr & 0xffffffc0;
	    if (index_empty_in_group == -1){
	        uint32_t random_line = index_for_group + (rand() % 8);
	        memcpy(Cache[random_line].block, hw_mem +start_addr , 64);
	        Cache[random_line].valid_byte =1;
	        Cache[random_line].sign_byte =sign_byte;
	    }
	    else{
	        memcpy(Cache[index_empty_in_group].block, hw_mem +start_addr , 64);
	        Cache[index_empty_in_group].valid_byte =1;
	        Cache[index_empty_in_group].sign_byte =sign_byte;
	    }
	}
}

// read data from cache
uint32_t cache_read(paddr_t paddr, size_t len)
{
	// implement me in PA 3-1
	uint32_t sign_byte = (paddr >> 13) & 0x7ffff;
	uint32_t index_for_group = (paddr >> 6) & 0x7f;
	uint32_t index_in_group = paddr & 0x3f;
	
	bool whether_hit = false;
	uint32_t index_empty_in_group = -1;
	uint32_t ans = 0;
	for (int i = 0; i < 8; i++){
	    uint32_t index_line = index_for_group * 8 + i;
	    if (Cache[index_line].valid_byte == 1) { //有效位有效
	        if (Cache[index_line].sign_byte == sign_byte){ //tmd命中了
	            whether_hit = true;
	            for (int j = 0; j < len; j++){ //开始一位位的读取
	                if (index_in_group + j >= 64) { //qnmd跨行了
	                    uint32_t ans_rest = cache_read(paddr + j, len - j);
	                    ans = ans | (ans_rest << (8*j));
	                    return ans;
	                }
	                else {
	                    ans = ans | (Cache[index_line].block[index_in_group + j] << (j*8));
	                }
	            }
	            return ans;
	        }
	    } else {
	        index_empty_in_group = index_line;
	    }
	}
	
	if (!whether_hit) {
	    memcpy(&ans, hw_mem + paddr, len);
	    uint32_t start_addr = paddr & 0xffffffc0;
	    if (index_empty_in_group != -1){
	        memcpy(Cache[index_empty_in_group].block, hw_mem + start_addr, 64);
	        Cache[index_empty_in_group].valid_byte = 1;
	        Cache[index_empty_in_group].sign_byte = sign_byte;
	    }
	    else{
	        uint32_t random_line = (rand() % 8) + index_for_group * 8;
	        memcpy(Cache[random_line].block, hw_mem + start_addr, 64);
	        Cache[random_line].valid_byte = 1;
	        Cache[random_line].sign_byte = sign_byte;
	    }
	}
	return ans;
}

