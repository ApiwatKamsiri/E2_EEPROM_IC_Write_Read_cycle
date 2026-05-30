/***********************************************************************************************************************
* Function Name	: system.c
* Description  	: This function implements system function.
* author    	: Apiwat Kamsiri
* Create date 	: 29-05-2026
***********************************************************************************************************************/

/***********************************************************************************************************************
Include
***********************************************************************************************************************/
#include "system.h"

/***********************************************************************************************************************
Define function
***********************************************************************************************************************/
static System_Registry_t SysRegistryInstance;

System_Registry_t *SysProc = &SysRegistryInstance;

/***********************************************************************************************************************
Initial state
***********************************************************************************************************************/
void SystemInitState(void)
{
	SysProc->SysDev.pTimer = &TimerProcSys;
	SysProc->SysProcess.pEep = &processCurrent;

	SysTimer_Init();		//Initial hardware timer
	EepromBackup_Init();	//Initial eeprom backuo
}

/***********************************************************************************************************************
System loop state
***********************************************************************************************************************/
void SystemRunTimerState(void)
{
	SysTimer_Channel_1ms activeTimer = SysTimer_GetActiveTimer();

	switch (activeTimer)
	    {
	        case SYS_TIMER_10MS:
					MainState();
					SysTimer_Set(SYS_TIMER_10MS, 10);
	            break;
	        default:
	        		SystemRunTimerStateDefault(activeTimer);
	            break;
	    }
}

/***********************************************************************************************************************
System loop state default
***********************************************************************************************************************/
void SystemRunTimerStateDefault(SysTimer_Channel_1ms activeTimer)
{
	switch (activeTimer)
	    {
	        case SYS_TIMER_10MS:
	        		MainState();
	            	SysTimer_Set(SYS_TIMER_10MS, 10);
	            break;
	        default:
	            break;
	    }
}


/***********************************************************************************************************************
Main loop state
***********************************************************************************************************************/
void MainState(void)
{
	EEPROMBackupStateHandler();
}
