/***********************************************************************************************************************
* Function Name	: timer.c
* Description  	: This function implements timer function.
* author    	: Apiwat Kamsiri
* Create date 	: 29-05-2026
***********************************************************************************************************************/

/***********************************************************************************************************************
Include
***********************************************************************************************************************/
#include "timer.h"
//Global variable
#include <stddef.h>
#include "iodefine.h"	//Special complier define
#include "r_cg_timer.h"	//Timer hardware define

/***********************************************************************************************************************
Define function
***********************************************************************************************************************/
SysTimer_type TimerProcSys;


/***********************************************************************************************************************
Pragma function
***********************************************************************************************************************/



/***********************************************************************************************************************
System timer initial function
***********************************************************************************************************************/
void SysTimer_Init(void)
{
		//Global variable
		R_TAU0_Channel4_Start();

		TimerProcSys.periodic1Ms.counter   = 0;
	    TimerProcSys.periodic1Ms.isTimeout = 0;

	    TimerProcSys.periodic10Ms.counter  = 0;
	    TimerProcSys.periodic10Ms.isTimeout = 0;

	    TimerProcSys.periodic20Ms.counter  = 0;
	    TimerProcSys.periodic20Ms.isTimeout = 0;

	    TimerProcSys.periodic100Ms.counter = 0;
	    TimerProcSys.periodic100Ms.isTimeout = 0;

	    TimerProcSys.periodic1Sec.counter  = 0;
	    TimerProcSys.periodic1Sec.isTimeout = 0;

	    TimerProcSys.periodic1Min.counter  = 0;
	    TimerProcSys.periodic1Min.isTimeout = 0;

	    TimerProcSys.periodic1Hour.counter = 0;
	    TimerProcSys.periodic1Hour.isTimeout = 0;

	    SysTimer_Set(SYS_TIMER_1MS, 1);
	    SysTimer_Set(SYS_TIMER_10MS, 10);
	    SysTimer_Set(SYS_TIMER_20MS, 20);
	    SysTimer_Set(SYS_TIMER_100MS, 20);
	    SysTimer_Set(SYS_TIMER_1SEC, 1000);
	    SysTimer_Set(SYS_TIMER_1MIN, 60 * 1000);
	    SysTimer_Set(SYS_TIMER_1HOUR, 60 * 60 * 1000);
}

/***********************************************************************************************************************
System timer initial function
***********************************************************************************************************************/
void SysTimer_Set(SysTimer_Channel_1ms channel, uint16_t durationMs)
{
	SysTimer_1ms *pTimer = NULL;

	//Timer case
	switch (channel)
	    {
	        case SYS_TIMER_1MS:
	        		pTimer = &TimerProcSys.periodic1Ms;
	        	break;
	        case SYS_TIMER_10MS:
	        		pTimer = &TimerProcSys.periodic10Ms;
	        	break;
	        case SYS_TIMER_20MS:
	        		pTimer = &TimerProcSys.periodic20Ms;
	        	break;
	        case SYS_TIMER_100MS:
	        		pTimer = &TimerProcSys.periodic100Ms;
	        	break;
	        case SYS_TIMER_1SEC:
	        		pTimer = &TimerProcSys.periodic1Sec;
	        	break;
	        case SYS_TIMER_1MIN:
	        		pTimer = &TimerProcSys.periodic1Min;
	        	break;
	        case SYS_TIMER_1HOUR:
	        		pTimer = &TimerProcSys.periodic1Hour;
	        	break;
	        default:
	        	return;
	    }

	//Load time value in the channel
    if (durationMs == 0)
    {
    	pTimer->counter = 0;
    	pTimer->isTimeout = 1;
    }
    else
    {
    	DI();	//Protect ticker during the loop
    	pTimer->counter = durationMs;
    	pTimer->isTimeout = 0;
        EI();	//Start
    }
}

/***********************************************************************************************************************
Timer Hardware Interrupt Service Routine (Every 1ms form CH4)
***********************************************************************************************************************/
void Timer_Hardware_ISR(void)
{
    if (TimerProcSys.periodic1Ms.counter > 0) {
        TimerProcSys.periodic1Ms.counter--;
        if (TimerProcSys.periodic1Ms.counter == 0) TimerProcSys.periodic1Ms.isTimeout = 1;
    }
    if (TimerProcSys.periodic10Ms.counter > 0) {
        TimerProcSys.periodic10Ms.counter--;
        if (TimerProcSys.periodic10Ms.counter == 0) TimerProcSys.periodic10Ms.isTimeout = 1;
    }
    if (TimerProcSys.periodic20Ms.counter > 0) {
        TimerProcSys.periodic20Ms.counter--;
        if (TimerProcSys.periodic20Ms.counter == 0) TimerProcSys.periodic20Ms.isTimeout = 1;
    }
    if (TimerProcSys.periodic100Ms.counter > 0) {
        TimerProcSys.periodic100Ms.counter--;
        if (TimerProcSys.periodic100Ms.counter == 0) TimerProcSys.periodic100Ms.isTimeout = 1;
    }
    if (TimerProcSys.periodic1Sec.counter > 0) {
        TimerProcSys.periodic1Sec.counter--;
        if (TimerProcSys.periodic1Sec.counter == 0) TimerProcSys.periodic1Sec.isTimeout = 1;
    }
    if (TimerProcSys.periodic1Min.counter > 0) {
        TimerProcSys.periodic1Min.counter--;
        if (TimerProcSys.periodic1Min.counter == 0) TimerProcSys.periodic1Min.isTimeout = 1;
    }
    if (TimerProcSys.periodic1Hour.counter > 0) {
        TimerProcSys.periodic1Hour.counter--;
        if (TimerProcSys.periodic1Hour.counter == 0) TimerProcSys.periodic1Hour.isTimeout = 1;
    }
}

/***********************************************************************************************************************
Timer scan timer completed
***********************************************************************************************************************/
SysTimer_Channel_1ms SysTimer_GetActiveTimer(void)
{
		//Scan follow as priority
		if (TimerProcSys.periodic1Ms.isTimeout == 1)
	    {
	        TimerProcSys.periodic1Ms.isTimeout = 0;
	        return SYS_TIMER_1MS;
	    }
	    else if (TimerProcSys.periodic10Ms.isTimeout == 1)
	    {
	        TimerProcSys.periodic10Ms.isTimeout = 0;
	        return SYS_TIMER_10MS;
	    }
	    else if (TimerProcSys.periodic20Ms.isTimeout == 1)
	    {
	        TimerProcSys.periodic20Ms.isTimeout = 0;
	        return SYS_TIMER_20MS;
	    }
	    else if (TimerProcSys.periodic100Ms.isTimeout == 1)
	    {
	        TimerProcSys.periodic100Ms.isTimeout = 0;
	        return SYS_TIMER_100MS;
	    }
	    else if (TimerProcSys.periodic1Sec.isTimeout == 1)
	    {
	        TimerProcSys.periodic1Sec.isTimeout = 0;
	        return SYS_TIMER_1SEC;
	    }
	    else if (TimerProcSys.periodic1Min.isTimeout == 1)
	    {
	        TimerProcSys.periodic1Min.isTimeout = 0;
	        return SYS_TIMER_1MIN;
	    }
	    else if (TimerProcSys.periodic1Hour.isTimeout == 1)
	    {
	        TimerProcSys.periodic1Hour.isTimeout = 0;
	        return SYS_TIMER_1HOUR;
	    }

return (SysTimer_Channel_1ms)-1;
}


