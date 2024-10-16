#ifndef __VIRTUAL_MEMORY_H__
#define __VIRTUAL_MEMORY_H__

extern void* pPhymem;

#define VIRTUAL_PAGE_SIZE (4096)
#define VIRTUAL_ADDRESS_BITS (32)
#define VIRTUAL_PAGE_OFFSET_MASK (0x00000fff)
#define VIRTUAL_PAGE_NUMBER_MASK (0xfffff000)


#endif