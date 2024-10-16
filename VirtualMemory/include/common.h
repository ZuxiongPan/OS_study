#ifndef __COMMON_H__
#define __COMMON_H__

#define NULL_POINTER ((void*)0)

#include "logger.h"
#include "physical_memory.h"

typedef int (*init_func_ptr)(void);
typedef void (*exit_func_ptr)(void);

init_func_ptr init_functions[] = 
{
	physical_memory_init,
	NULL_POINTER,
};

exit_func_ptr exit_functions[] = 
{
	physical_memory_exit,
	NULL_POINTER,
};

#endif