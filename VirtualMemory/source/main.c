#include "common.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

void do_initcalls(void)
{
	int i = 0, ret = 0;

	if(log_system_init() < 0)	// log system init first
	{
		printf("log system work execption\n");
		exit(EXIT_FAILURE);
	}

	while(NULL_POINTER != init_functions[i])
	{
		ret = init_functions[i]();
		if(ret < 0)
		{
			log_error("init function index %d error\n", i);
			exit(EXIT_FAILURE);
		}
		i++;
	}
	log_message("the Virtual Memory System init OK!\n");

	return ;
}

void do_exitcalls(void)
{
	int i = 0;

	while(NULL_POINTER != exit_functions[i])
	{
		exit_functions[i]();
		i++;
	}
	log_message("Exit the Virtual Memory System\n");

	log_system_exit();

	return ;	
}

int main(int argc, const char* argv[])
{
	do_initcalls();

	char str[] = "Hello World\n";
	int i = 0;
	struct PhysicalPageHeader* header = NULL;

	for(i = 0; i < 256; i++)
	{
		unsigned int virtAddr = 0x20000000 + i * PHYSICAL_PAGE_FRAME_SIZE;
		void* phyAddr = allocate_physical_page(virtAddr);
		if(phyAddr == NULL_POINTER)
			log_error("cannot get memory\n");
		else
		{
			log_debug("memory request success, physical address is %x\n", phyAddr - pPhyMem);
			header = (struct PhysicalPageHeader*)phyAddr;
			log_debug("header: magic-0x%x physicalAddress-0x%x virtualAddress-0x%x pageAvaiableSize-0x%x realPageStart-0x%x\n", \
				header->magic, header->physicalAddress, header->virtualAddress, header->pageAvaiableSize, header->realPageStart);
			strcpy(header->realPageStart + i * 8, str);
		}
	}

	for(i = 0; i < 256; i += 2)
		collect_physical_page(i * PHYSICAL_PAGE_FRAME_SIZE);

	recycle_physical_memory_work();

	do_exitcalls();

	return 0;
}