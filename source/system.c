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
	SysProc->SysDev.pI2C = &I2CProcSys;

	SysProc->SysProcess.pEep = &processCurrent;

	//Hardware
	SysTimer_Init();		//Initial hardware timer
	SysI2C_Init();			//Initial hardware i2c

	//Functiuon
	EepromBackup_Init();	//Initial eeprom backup

}

/***********************************************************************************************************************
System loop state
***********************************************************************************************************************/
void SystemRunTimerState(void)
{
	SysTimer_Channel_1ms activeTimer = SysTimer_GetActiveTimer();

	//Global loop
	MainState();

	//Timer loop
	switch (activeTimer)
	    {
			case SYS_TIMER_1HOUR:
			case SYS_TIMER_1MIN:
			case SYS_TIMER_1SEC:
			case SYS_TIMER_100MS:
			case SYS_TIMER_20MS:
			case SYS_TIMER_10MS:
			case SYS_TIMER_1MS:
					;
				break;

	        default:
	        		;
	            break;
	    }
}


/***********************************************************************************************************************
Main loop state
***********************************************************************************************************************/
void MainState(void)
{
	//Global loop
	EEPROMBackupStateHandler();
}
