#ifndef __PHYSICAL_MEMORY_H__
#define __PHYSICAL_MEMORY_H__

#define PHYSICAL_PAGE_HEAD_MAGIC (0x114477AA)

#define PHYSICAL_MEMORY_SIZE (128 * 1024 * 1024)	// 128M Bytes
#define PHYSICAL_PAGE_FRAME_SIZE (4096)	// 4096 Bytes
#define PHYSICAL_PAGE_FRAME_NUMBER (PHYSICAL_MEMORY_SIZE / PHYSICAL_PAGE_FRAME_SIZE)	// 32768 physical pages
#define PHYSICAL_PAGE_FRAME_BITMAP_ELEMENTS (PHYSICAL_PAGE_FRAME_NUMBER / 8)	// needs 4096 Bytes save bitmap
#define BIT_MASK(BIT) (0x1 << (BIT % 8))
#define ACCUMULATE_MIN_PAGES (256)	// if recycle 256 pages(1 MB), stop recycle

#define PHYSICAL_PAGE_UNUSED (0x0)
#define PHYSICAL_PAGE_INUSE (0x1)
#define PHYSICAL_PAGE_RESERVED (0x2)

extern void* pPhyMem;

struct PhysicalPageHeader
{
	unsigned int magic;			// if allcated, magic is PHYSICAL_PAGE_HEAD_MAGIC
	unsigned int flags;			// some flags for ideentity
	unsigned int referenceCount;	// how many objects use this page
	unsigned int physicalAddress;	// real physical address in memory
	unsigned int virtualAddress;	// mapped virtual address
	unsigned int pageAvaiableSize;	// PHYSICAL_PAGE_FRAME_SIZE - sizeof(struct PhysicalPageHeader)
	void* realPageStart;	// physical address + sizeof(struct PhysicalPageHeader)
};

int physical_memory_init(void);
void physical_memory_exit(void);

void* allocate_physical_page(unsigned int virtualAddress);	// direct use by dereference the return value's realPageStart
void collect_physical_page(unsigned int physicalAddress);
void recycle_physical_memory_work(void);

#endif