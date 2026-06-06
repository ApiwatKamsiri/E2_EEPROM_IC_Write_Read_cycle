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
#include "..\\dev\i2c.h"

/***********************************************************************************************************************
Define function
***********************************************************************************************************************/
//Global variable
EepromProcessStruct_t processCurrent;

#define slave_address 80
#define EEPROM_PAGE_SIZE 32

static uint8_t PowerON;

/***********************************************************************************************************************
Initial EEPROM backup function
***********************************************************************************************************************/
void EepromBackup_Init(void)
{
	processCurrent.process = EEPROM_PROCESS_NONE;
	PowerON = 1;
}


/***********************************************************************************************************************
EEPROM backup Process
***********************************************************************************************************************/
void EEPROMBackupStateHandler(void)
{
	SysTimer_Channel_1ms activeTimer = SysTimer_GetActiveTimer();

	//Global variable
	UpdateEEPROMTask();

	switch (activeTimer)
	    {
			case SYS_TIMER_1MIN:
					EEPROM1MINTask();
				break;
	        case SYS_TIMER_1SEC:
					EEPROM1sTask();
	            break;
	        case SYS_TIMER_20MS:
					EEPROM20MsTask();
	            break;
			case SYS_TIMER_10MS:
					EEPROM10MsTask();
				break;
	        default:
	        		;
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

void UpdateEEPROMTask(void)
{
	;
}
