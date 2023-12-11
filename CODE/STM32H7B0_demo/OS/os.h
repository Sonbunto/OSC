#ifndef __OS_H
#define __OS_H
#include "bsp.h"

#define OS_OK    (0)
#define OS_ERR   (1)

#ifndef _VC_SIMULATOR_

#define OS_PRIORITY_MAX  (0x10)

typedef struct
{
	char * name;
	int (*os_init)(void);
	int (*os_config)(void);
	uint8_t seq_type;
}os_init_t;

typedef enum
{
	PRIORITY_0 = 0,
	PRIORITY_1 = 1,
	PRIORITY_2 = 2,
	PRIORITY_3 = 3,
	PRIORITY_4 = 4,
	PRIORITY_IDLE = 5,

}os_priority;

typedef struct os_tsk_link
{
	struct os_tsk_link * linker;
	void (*enter)(void);
	unsigned int period;
	os_priority priority;
	
}os_tsk_t;


extern os_init_t os_init_ram$$Base;
extern os_init_t os_init_ram$$Limit;
#define OS_INIT_BASE    ((os_init_t *)&os_init_ram$$Base)
#define OS_INIT_LIMIT  ((os_init_t *)&os_init_ram$$Limit)
#define OS_INIT_NUM   (&os_init_ram$$Limit-&os_init_ram$$Base)

extern os_tsk_t os_tsk_ram$$Base;
extern os_tsk_t os_tsk_ram$$Limit;
#define OS_TSK_BASE    ((os_tsk_t *)&os_tsk_ram$$Base)
#define OS_TSK_LIMIT  ((os_tsk_t *)&os_tsk_ram$$Limit)
#define OS_TSK_NUM   (&os_tsk_ram$$Limit-&os_tsk_ram$$Base)


#define __INIT 

#define OS_INIT_REGISTER(name,init,config,st)                            	\
const os_init_t __FS_##init                                                 \
__attribute__((unused, section("os_init_ram"))) =                           \
{                         												    	\
	name,																		\
	init,																		\
	config,																		\
	st																			\
}																				\

//#define __INIT__TSK __attribute__((unused,section("os_tsk_ram")))

#define OS_TSK_REGISTER(entrance,priority,period)							\
static os_tsk_t __OS_TSK##entrance											\
__attribute__((used,section("os_tsk_ram"))) = 								\
{																				\
	(void *)0, 																	\
	entrance,																	\
	period,																		\
	priority																	\
}																				\

uint8_t os_dev_init(void);
void run_thead_priority_idle(void);
int thread_timer_init(void);
int thread_start_it(void);

#endif


#endif
