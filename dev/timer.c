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

/***********************************************************************************************************************
Define function
***********************************************************************************************************************/
/* Timer device system variable */
SysTimer_type TimerDevProcSys;


/***********************************************************************************************************************
System timer initial function
***********************************************************************************************************************/
void SysTimer_Init(void)
{
		/* Global hartware variable */
		R_TAU0_Channel4_Start();

		/* Timer initial */
		TimerDevProcSys.periodic1Ms.counter   = 0;
		TimerDevProcSys.periodic1Ms.isTimeout = 0;

		TimerDevProcSys.periodic10Ms.counter  = 0;
		TimerDevProcSys.periodic10Ms.isTimeout = 0;

		TimerDevProcSys.periodic20Ms.counter  = 0;
		TimerDevProcSys.periodic20Ms.isTimeout = 0;

		TimerDevProcSys.periodic100Ms.counter = 0;
		TimerDevProcSys.periodic100Ms.isTimeout = 0;

		TimerDevProcSys.periodic1Sec.counter  = 0;
		TimerDevProcSys.periodic1Sec.isTimeout = 0;

		TimerDevProcSys.periodic1Min.counter  = 0;
		TimerDevProcSys.periodic1Min.isTimeout = 0;

		TimerDevProcSys.periodic1Hour.counter = 0;
		TimerDevProcSys.periodic1Hour.isTimeout = 0;

		/* Start timer initial */
	    SysTimer_Set(SYS_TIMER_1MS, Value_1MS);
	    SysTimer_Set(SYS_TIMER_10MS, Value_10MS);
	    SysTimer_Set(SYS_TIMER_20MS, Value_20MS);
	    SysTimer_Set(SYS_TIMER_100MS, Value_100MS);
	    SysTimer_Set(SYS_TIMER_1SEC, Value_1S);
	    SysTimer_Set(SYS_TIMER_1MIN, Value_1Min);
	    SysTimer_Set(SYS_TIMER_1HOUR, Value_1Hr);
}

/***********************************************************************************************************************
System timer function
***********************************************************************************************************************/
void SysTimer_Set(SysTimer_Channel_1ms channel, uint32_t durationMs)
{
	SysTimer_1ms *devTimer = NULL;

	/* Timer system operation */
	switch (channel)
	    {
	        case SYS_TIMER_1MS:
	        		devTimer = &TimerDevProcSys.periodic1Ms;
	        	break;
	        case SYS_TIMER_10MS:
	        		devTimer = &TimerDevProcSys.periodic10Ms;
	        	break;
	        case SYS_TIMER_20MS:
	        		devTimer = &TimerDevProcSys.periodic20Ms;
	        	break;
	        case SYS_TIMER_100MS:
	        		devTimer = &TimerDevProcSys.periodic100Ms;
	        	break;
	        case SYS_TIMER_1SEC:
	        		devTimer = &TimerDevProcSys.periodic1Sec;
	        	break;
	        case SYS_TIMER_1MIN:
	        		devTimer = &TimerDevProcSys.periodic1Min;
	        	break;
	        case SYS_TIMER_1HOUR:
	        		devTimer = &TimerDevProcSys.periodic1Hour;
	        	break;
	        default:
	        		devTimer = NULL;
	        	break;
	    }

	/* Load time value in the channel */
    if (durationMs == 0)
    {
    	devTimer->counter = 0;
    	devTimer->isTimeout = 1;
    }
    else
    {
    	DI();	//Protect ticker during the loop
    	devTimer->reload = durationMs;
    	devTimer->counter = durationMs;
    	devTimer->isTimeout = 0;
        EI();	//Start
    }
}


/***********************************************************************************************************************
Timer Hardware Interrupt Service Routine (Every 1ms form CH4)
***********************************************************************************************************************/
void Timer_Hardware_ISR(void)
{
    if (TimerDevProcSys.periodic1Ms.counter > 0) {
    	TimerDevProcSys.periodic1Ms.counter--;
        if (TimerDevProcSys.periodic1Ms.counter == 0)
        {
        	TimerDevProcSys.periodic1Ms.isTimeout = 1;
        	TimerDevProcSys.periodic1Ms.counter = TimerDevProcSys.periodic1Ms.reload;
        }
    }
    if (TimerDevProcSys.periodic10Ms.counter > 0) {
    	TimerDevProcSys.periodic10Ms.counter--;
        if (TimerDevProcSys.periodic10Ms.counter == 0)
        {
        	TimerDevProcSys.periodic10Ms.isTimeout = 1;
        	TimerDevProcSys.periodic10Ms.counter = TimerDevProcSys.periodic10Ms.reload;
        }
    }
    if (TimerDevProcSys.periodic20Ms.counter > 0) {
        TimerDevProcSys.periodic20Ms.counter--;
        if (TimerDevProcSys.periodic20Ms.counter == 0)
        {
        	TimerDevProcSys.periodic20Ms.isTimeout = 1;
        	TimerDevProcSys.periodic20Ms.counter = TimerDevProcSys.periodic20Ms.reload;
        }
    }
    if (TimerDevProcSys.periodic100Ms.counter > 0) {
        TimerDevProcSys.periodic100Ms.counter--;
        if (TimerDevProcSys.periodic100Ms.counter == 0)
        {
        	TimerDevProcSys.periodic100Ms.isTimeout = 1;
        	TimerDevProcSys.periodic100Ms.counter = TimerDevProcSys.periodic100Ms.reload;
        }

    }
    if (TimerDevProcSys.periodic1Sec.counter > 0) {
        TimerDevProcSys.periodic1Sec.counter--;
        if (TimerDevProcSys.periodic1Sec.counter == 0)
        {
        	TimerDevProcSys.periodic1Sec.isTimeout = 1;
        	TimerDevProcSys.periodic1Sec.counter = TimerDevProcSys.periodic1Sec.reload;
        }
    }
    if (TimerDevProcSys.periodic1Min.counter > 0) {
        TimerDevProcSys.periodic1Min.counter--;
        if (TimerDevProcSys.periodic1Min.counter == 0)
        {
        	TimerDevProcSys.periodic1Min.isTimeout = 1;
        	TimerDevProcSys.periodic1Min.counter = TimerDevProcSys.periodic1Min.reload;
        }
    }
    if (TimerDevProcSys.periodic1Hour.counter > 0) {
        TimerDevProcSys.periodic1Hour.counter--;
        if (TimerDevProcSys.periodic1Hour.counter == 0)
        {
        	TimerDevProcSys.periodic1Hour.isTimeout = 1;
        	TimerDevProcSys.periodic1Hour.counter = TimerDevProcSys.periodic1Hour.reload;
        }
    }
}


/***********************************************************************************************************************
Timer scan timer completed
***********************************************************************************************************************/
SysTimer_Channel_1ms SysTimer_GetActiveTimer(void)
{
		/* Scan timer completed */
		if (TimerDevProcSys.periodic1Ms.isTimeout == 1)
	    {
	        TimerDevProcSys.periodic1Ms.isTimeout = 0;
	        SysTimer_1Ms();
	        return SYS_TIMER_1MS;
	    }
	    if (TimerDevProcSys.periodic10Ms.isTimeout == 1)
	    {
	        TimerDevProcSys.periodic10Ms.isTimeout = 0;
	        SysTimer_10Ms();
	        return SYS_TIMER_10MS;
	    }
	    if (TimerDevProcSys.periodic20Ms.isTimeout == 1)
	    {
	        TimerDevProcSys.periodic20Ms.isTimeout = 0;
	        SysTimer_20Ms();
	        return SYS_TIMER_20MS;
	    }
	    if (TimerDevProcSys.periodic100Ms.isTimeout == 1)
	    {
	        TimerDevProcSys.periodic100Ms.isTimeout = 0;
	        SysTimer_100Ms();
	        return SYS_TIMER_100MS;
	    }
	    if (TimerDevProcSys.periodic1Sec.isTimeout == 1)
	    {
	        TimerDevProcSys.periodic1Sec.isTimeout = 0;
	        SysTimer_1SEC();
	        return SYS_TIMER_1SEC;
	    }
	    if (TimerDevProcSys.periodic1Min.isTimeout == 1)
	    {
	        TimerDevProcSys.periodic1Min.isTimeout = 0;
	        SysTimer_1MIN();
	        return SYS_TIMER_1MIN;
	    }
	    if (TimerDevProcSys.periodic1Hour.isTimeout == 1)
	    {
	        TimerDevProcSys.periodic1Hour.isTimeout = 0;
	        SysTimer_1HOUR();
	        return SYS_TIMER_1HOUR;
	    }

return (SysTimer_Channel_1ms) - 1;
}
