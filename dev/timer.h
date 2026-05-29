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
#include "r_cg_macrodriver.h"


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
    uint16_t counter;
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
//Global variable
extern SysTimer_type TimerProcSys;

extern void SysTimer_Init(void);
extern void SysTimer_Set(SysTimer_Channel_1ms channel, uint16_t durationMs);
extern void Timer_Hardware_ISR(void);
extern SysTimer_Channel_1ms SysTimer_GetActiveTimer(void);


#endif
