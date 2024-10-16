#include "physical_memory.h"
#include "logger.h"
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>

const char phyMemFile[] = "/tmp/memfile.bin";
int fd = -1;
void* pPhyMem = NULL;
unsigned char* phyBitMap;

static unsigned int find_free_physical_page(void);
static void recycle_physical_memory(void);

int physical_memory_init(void)
{
	if(fd < 0)
	{
		fd = open(phyMemFile, (O_RDWR | O_CREAT), (S_IRUSR | S_IWUSR));
		if(fd < 0)
		{
			log_error("open file %s failed\n", phyMemFile);
			return -1;
		}
	}

	if(ftruncate(fd, PHYSICAL_MEMORY_SIZE) < 0)
	{
		log_error("file %s init failed\n", phyMemFile);
		return -1;
	}

	pPhyMem = mmap(NULL, PHYSICAL_MEMORY_SIZE, (PROT_READ | PROT_WRITE), MAP_SHARED, fd, 0);
	if(pPhyMem == MAP_FAILED)
	{
		pPhyMem = NULL;
		log_error("mmap file %s failed\n", phyMemFile);
		return -1;
	}

	log_debug("pPhyMem Address 0x%x\n", pPhyMem);
	// init physical memory bitmap
	// bit'1-used bit'0-free
	phyBitMap = (unsigned char*)pPhyMem;
	phyBitMap[0] = BIT_MASK(0);
	for(int i = 1; i < PHYSICAL_PAGE_FRAME_BITMAP_ELEMENTS; i++)
		phyBitMap[i] = 0x0;

	return 0;
}

void physical_memory_exit(void)
{
	if(pPhyMem != NULL)
		if(munmap(pPhyMem, PHYSICAL_MEMORY_SIZE) < 0)
			log_error("munmap file %s failed\n", phyMemFile);

	if(fd > 0)
	{
		close(fd);
		fd = -1;
	}

	return ;
}

// this function return the address of memfile in host memory
void* allocate_physical_page(unsigned int virtualAddress)
{
	unsigned int ret = find_free_physical_page();
	struct PhysicalPageHeader* header = NULL;

	if(ret == 0)
	{
		log_debug("here is not enough physical memory\n");
		recycle_physical_memory();
		return (void*)0;
	}

	// init physical page
	header = (struct PhysicalPageHeader*)(pPhyMem + ret);
	header->magic = PHYSICAL_PAGE_HEAD_MAGIC;
	header->flags = PHYSICAL_PAGE_INUSE;
	header->referenceCount = 1;
	header->physicalAddress = ret;
	header->virtualAddress = virtualAddress;
	header->pageAvaiableSize = PHYSICAL_PAGE_FRAME_SIZE - sizeof(struct PhysicalPageHeader);
	header->realPageStart = pPhyMem + ret + sizeof(struct PhysicalPageHeader);

	return (void*)(pPhyMem + ret);
}

// this function set the page with the offest of memfile.bin
void collect_physical_page(unsigned int physicalAddress)
{
	// this function only set flag, memory recycle will be done in other place
	struct PhysicalPageHeader* header = (struct PhysicalPageHeader*)(pPhyMem + physicalAddress);
	if(header->magic == PHYSICAL_PAGE_HEAD_MAGIC)
	{
		header->referenceCount --;
		if(header->referenceCount == 0)
			header->flags = PHYSICAL_PAGE_UNUSED;
	}
	else
		log_error("this page %x is incorrect or cannot collect\n", physicalAddress);

	return ;
}

void recycle_physical_memory_work(void)
{
	recycle_physical_memory();
}

// this function return offset of memfile.bin
static unsigned int find_free_physical_page(void)
{
	int i = 0;	// page 0 is always unused, it doesn't have header but only bitmap
	int bit = 0, temp = 0;
	int ret = 0, flag = 0;
	for( ; (i < PHYSICAL_PAGE_FRAME_BITMAP_ELEMENTS) && (!flag); i++)
	{
		if(phyBitMap[i] != 0xff)
		{
			temp = phyBitMap[i];
			while(bit < 8)
			{
				if(((temp & 0x1) | 0x0) == 0)
				{
					flag = 1;
					ret = (i * 8 + bit) * PHYSICAL_PAGE_FRAME_SIZE;
					// set bitmap
					phyBitMap[i] |= BIT_MASK(bit);
					break;
				}
				bit++;
				temp >>= 1;
			}
		}
	}

	return ret;
}

static void recycle_physical_memory(void)
{
	static unsigned int lastScanPosition = 0;
	int bit = 0, temp = 0, accumulate = 0;
	unsigned int physicalAddress = 0;

	log_debug("start recycle physical memory\n");
	for( ; (lastScanPosition < PHYSICAL_PAGE_FRAME_BITMAP_ELEMENTS) && (accumulate < ACCUMULATE_MIN_PAGES);\
		lastScanPosition = lastScanPosition % PHYSICAL_PAGE_FRAME_BITMAP_ELEMENTS + 1)
	{
		if(phyBitMap[lastScanPosition] != 0x0)
		{
			temp = phyBitMap[lastScanPosition];
			bit = 0;
			while(bit < 8)
			{
				if((temp & 0x1) == 1)
				{
					physicalAddress = (lastScanPosition * 8 + bit) * PHYSICAL_PAGE_FRAME_SIZE;
					// judge page and recycle property
					struct PhysicalPageHeader* header = (struct PhysicalPageHeader*)(pPhyMem + physicalAddress);
					if((header->magic == PHYSICAL_PAGE_HEAD_MAGIC) && (header->flags == PHYSICAL_PAGE_UNUSED))
					{
						log_debug("physical address %x is free, recycle\n", physicalAddress);
						memset((void*)header, 0, PHYSICAL_PAGE_FRAME_SIZE);
						strcpy((char*)header, "this memory was used but already recycled\n");
						// set bitmap
						phyBitMap[lastScanPosition] &= ~(BIT_MASK(bit));
						accumulate ++;
					}
				}
				bit++;
				temp >>= 1;
			}
		}
	}

	return ;
}