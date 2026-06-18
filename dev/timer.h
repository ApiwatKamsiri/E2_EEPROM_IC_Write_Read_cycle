/***********************************************************************************************************************
* Function Name	: timer.h
* Description  	: This function implements timer function.
* author    	: Apiwat Kamsiri
* Create date 	: 29-05-2026
***********************************************************************************************************************/

#ifndef TIMER_H
#define TIMER_H

/***********************************************************************************************************************
Include function
***********************************************************************************************************************/
/* Global variable */
#include <stddef.h>
#include "iodefine.h"			//Special complier define
#include "r_cg_timer.h"			//Timer hardware define
#include "r_cg_macrodriver.h"

#define Value_1MS 1
#define Value_10MS 10
#define Value_20MS 20
#define Value_100MS 100
#define Value_1S 1000
#define Value_1Min 60UL* 1000UL
#define Value_1Hr 60UL * 60UL * 1000UL

/***********************************************************************************************************************
typedef counting function
***********************************************************************************************************************/
typedef enum {
    SYS_TIMER_1MS = 0,
    SYS_TIMER_10MS,
    SYS_TIMER_20MS,
    SYS_TIMER_100MS,
    SYS_TIMER_1SEC,
    SYS_TIMER_1MIN,
    SYS_TIMER_1HOUR
} SysTimer_Channel_1ms;

typedef struct {
	uint32_t counter;
    uint32_t reload;
    uint8_t  isTimeout;
} SysTimer_1ms;

typedef struct
{
	SysTimer_1ms periodic1Ms;
	SysTimer_1ms periodic10Ms;
	SysTimer_1ms periodic20Ms;
	SysTimer_1ms periodic100Ms;
	SysTimer_1ms periodic1Sec;
	SysTimer_1ms periodic1Min;
	SysTimer_1ms periodic1Hour;
} SysTimer_type;

/***********************************************************************************************************************
Define function
***********************************************************************************************************************/
/* Timer device system variable */
extern SysTimer_type TimerDevProcSys;

extern void SysTimer_Init(void);
extern void SysTimer_Set(SysTimer_Channel_1ms channel, uint32_t durationMs);
extern void Timer_Hardware_ISR(void);
extern SysTimer_Channel_1ms SysTimer_GetActiveTimer(void);

extern void SysTimer_1Ms(void);
extern void SysTimer_10Ms(void);
extern void SysTimer_20Ms(void);
extern void SysTimer_100Ms(void);
extern void SysTimer_1SEC(void);
extern void SysTimer_1MIN(void);
extern void SysTimer_1HOUR(void);

#endif
