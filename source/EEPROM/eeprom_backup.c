/***********************************************************************************************************************
* Function Name	: eeprom_backup.c
* Description  	: This function implements eeprom backup function.
* author    	: Apiwat Kamsiri
* Create date 	: 29-05-2026
***********************************************************************************************************************/

/***********************************************************************************************************************
Include
***********************************************************************************************************************/
#include <EEPROM\eeprom_backup.h>
#include "..\\dev\timer.h"

/***********************************************************************************************************************
Define function
***********************************************************************************************************************/
//Global variable
EepromProcessStruct_t processCurrent;



/***********************************************************************************************************************
Initial EEPROM backup function
***********************************************************************************************************************/
void EepromBackup_Init(void)
{
	processCurrent.process = EEPROM_PROCESS_NONE;
}


/***********************************************************************************************************************
EEPROM backup Process
***********************************************************************************************************************/
void EEPROMBackupStateHandler(void)
{
	SysTimer_Channel_1ms activeTimer = SysTimer_GetActiveTimer();

	switch (activeTimer)
	    {
			case SYS_TIMER_10MS:
					EEPROM10MsTask();
					SysTimer_Set(SYS_TIMER_10MS, 10);
				break;
	        case SYS_TIMER_20MS:
					EEPROM20MsTask();
					SysTimer_Set(SYS_TIMER_20MS, 20);
	            break;

	        case SYS_TIMER_1SEC:
					EEPROM1sTask();
					SysTimer_Set(SYS_TIMER_1SEC, 1000);
	            break;

	        case SYS_TIMER_1MIN:
					EEPROM1MINTask();
					SysTimer_Set(SYS_TIMER_1MIN, 60000);
	            break;

	        default:
	        		EEPROMBackupStateDefault(activeTimer);
	            break;
	    }
}

/***********************************************************************************************************************
EEPROM backup Process Default
***********************************************************************************************************************/
void EEPROMBackupStateDefault(SysTimer_Channel_1ms activeTimer)
{
	switch (activeTimer)
	    {
			case SYS_TIMER_10MS:
					EEPROM10MsTask();
					SysTimer_Set(SYS_TIMER_10MS, 10);
				break;

	        case SYS_TIMER_20MS:
	        		EEPROM20MsTask();
	            	SysTimer_Set(SYS_TIMER_20MS, 20);
	            break;

	        case SYS_TIMER_1SEC:
	        		EEPROM1sTask();
	            	SysTimer_Set(SYS_TIMER_1SEC, 1000);
	            break;

	        case SYS_TIMER_1MIN:
	        		EEPROM1MINTask();
	            	SysTimer_Set(SYS_TIMER_1MIN, 60000);
	            break;
	        default:
	            break;
	    }
}

/***********************************************************************************************************************
EEPROM timer task
***********************************************************************************************************************/
static void EEPROM10MsTask(void)
{
	;
}
static void EEPROM20MsTask(void)
{
	;
}
static void EEPROM1sTask(void)
{
	;
}
static void EEPROM1MINTask(void)
{
	;
}
