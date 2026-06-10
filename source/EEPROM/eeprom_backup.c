/***********************************************************************************************************************
* Function Name	: eeprom_backup.c
* Description  	: This function implements eeprom backup function.
* author    	: Apiwat Kamsiri
* Create date 	: 29-05-2026
***********************************************************************************************************************/

/***********************************************************************************************************************
Include
***********************************************************************************************************************/
#include <string.h>
#include <EEPROM\eeprom_backup.h>
#include "..\\dev\timer.h"
#include "..\\dev\i2c.h"

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
	processCurrent.workstate = EEPROM_STATE_Init;
	processCurrent.status.FirstPlug = 1;
	processCurrent.status.GetStatusComplete = 0;
	memset(processCurrent.status.Current_buffer, 0, EEPROM_PAGE_SIZE);
}


/***********************************************************************************************************************
EEPROM backup Process
***********************************************************************************************************************/
void EEPROMBackupStateHandler(void)
{
	//Global variable
	UpdateEEPROMTask();
}

void UpdateEEPROMTask(void)
{

	//Main operation
	if(processCurrent.status.FirstPlug)
	{
		processCurrent.process = EEPROM_PROCESS_UPDATE_Reading;
		processCurrent.status.CommandRead = 1;
		processCurrent.status.FirstPlug = 0;
	}
	else
	{
		if(processCurrent.status.CommandWrite && !processCurrent.status.CommandRead)
			processCurrent.process = EEPROM_PROCESS_UPDATE_Writing;
		else if(processCurrent.status.CommandRead && !processCurrent.status.CommandWrite)
			processCurrent.process = EEPROM_PROCESS_UPDATE_Reading;
	}

	//Sub operation


	// I2C Get status function
	if (SysI2C_Dev_GetStatus() == I2C_STATUS_BUSY_COMPLETED)
		processCurrent.status.GetStatusComplete = 1;
}

/***********************************************************************************************************************
EEPROM timer task
***********************************************************************************************************************/
void EEPROM1MsTask(void)
{;}

void EEPROM10MsTask(void)
{
	switch (processCurrent.process)
	{
		case EEPROM_PROCESS_NONE:
				processCurrent.workstate = EEPROM_STATE_NONE;
			break;

		case EEPROM_PROCESS_UPDATE_Writing:
				switch (processCurrent.workstate)
				{
					case EEPROM_STATE_NONE:
							if(processCurrent.process == EEPROM_PROCESS_UPDATE_Writing)
								processCurrent.workstate = EEPROM_STATE_Init;
							else
								processCurrent.workstate = EEPROM_STATE_NONE;
						break;

					case EEPROM_STATE_Init:
						//Test
						SysI2C_Dev_Write(SLAVE_ADDRESS, START_READ_ADDRESS, processCurrent.status.Current_buffer, EEPROM_PAGE_SIZE);
						break;

					case EEPROM_STATE_PrepareData:
						break;

					case EEPROM_STATE_Writing_Reading:
						break;

					case EEPROM_STATE_GetData:
						break;

					case EEPROM_STATE_Check:
						break;

					case EEPROM_STATE_Completed:
							processCurrent.status.CommandWrite = 0;
							processCurrent.process = EEPROM_PROCESS_UPDATE_COMPLETE;
						break;

					default:
						break;
				}
			break;

		case EEPROM_PROCESS_UPDATE_Reading:
				switch (processCurrent.workstate)
				{
					case EEPROM_STATE_NONE:
							if(processCurrent.process == EEPROM_PROCESS_UPDATE_Reading)
								processCurrent.workstate = EEPROM_STATE_Init;
							else
								processCurrent.workstate = EEPROM_STATE_NONE;
						break;

					case EEPROM_STATE_Init:
							processCurrent.workstate = EEPROM_STATE_PrepareData;
							I2CProcSys.status = I2C_STATUS_IDLE;
						break;

					case EEPROM_STATE_PrepareData:
							processCurrent.workstate = EEPROM_STATE_Writing_Reading;
						break;

					case EEPROM_STATE_Writing_Reading:
							SysI2C_Dev_Read(SLAVE_ADDRESS, START_READ_ADDRESS, processCurrent.status.Backup_buffer, EEPROM_MAX_ADDR);

							if (processCurrent.status.GetStatusComplete)
							{
								processCurrent.status.GetStatusComplete = 0;
								processCurrent.workstate = EEPROM_STATE_GetData;
								I2CProcSys.status = I2C_STATUS_NONE;
							}
						break;

					case EEPROM_STATE_GetData:
							processCurrent.workstate = EEPROM_STATE_Check;
						break;

					case EEPROM_STATE_Check:
							processCurrent.workstate = EEPROM_STATE_Completed;
						break;

					case EEPROM_STATE_Completed:
							processCurrent.status.CommandRead = 0;
							processCurrent.process = EEPROM_PROCESS_UPDATE_COMPLETE;
						break;

					default:
						break;
				}
			break;

		case EEPROM_PROCESS_UPDATE_COMPLETE:
				processCurrent.process = EEPROM_PROCESS_NONE;
			break;

		default:
			break;
	}
}
void EEPROM20MsTask(void)
{;}

void EEPROM1sTask(void)
{
	if(!processCurrent.status.CommandWrite && !processCurrent.status.CommandRead)
	{
		processCurrent.status.CommandWrite = 1;
	}
}
void EEPROM1MINTask(void)
{;}
