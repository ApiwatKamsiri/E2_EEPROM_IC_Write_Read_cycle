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

/* Global variable */
EepromProcessStruct_t processCurrent;

/* Normal variable */
#define StartWriteTime_set 2
#define StartDataWrite_location 0
#define StartDataAddress_location 4079
#define StartDataCountCycle_set 4095




/***********************************************************************************************************************
Initial EEPROM backup function
***********************************************************************************************************************/
void EepromBackup_Init(void)
{
	/* Initial state */
	processCurrent.process = EEPROM_PROCESS_NONE;
	processCurrent.workstate = EEPROM_STATE_Init;
	processCurrent.status.FirstPlug = 1;//0;//
	processCurrent.status.GetStatusComplete = 0;
	processCurrent.status.WriteCycleTime_1s = 0;

	/* Reset buffer function */
	memset(processCurrent.status.Current_buffer, 0, EEPROM_PAGE_SIZE);
}


/***********************************************************************************************************************
EEPROM backup Process
***********************************************************************************************************************/
void EEPROMBackupStateHandler(void)
{
	/* Global variable */
	UpdateEEPROMTask();
}

void UpdateEEPROMTask(void)
{

	/* Main operation */
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
		else if(processCurrent.status.CommandWrite && processCurrent.status.CommandRead)
			processCurrent.process = EEPROM_PROCESS_UPDATE_Reading;
	}

	/* Normal operation */
	if(processCurrent.status.WriteCycleTime_1s >= StartWriteTime_set)
	{
		if(!processCurrent.status.CommandWrite && !processCurrent.status.CommandRead)
		{
			processCurrent.status.CommandWrite = 1;
		}

		processCurrent.status.WriteCycleTime_1s = 0;
	}

	/* I2C Get status function */
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
				/* Write process operation */
				switch (processCurrent.workstate)
				{
					case EEPROM_STATE_NONE:
							/* Change state work operation */
							if(processCurrent.process == EEPROM_PROCESS_UPDATE_Writing)
								processCurrent.workstate = EEPROM_STATE_Init;
							else
								processCurrent.workstate = EEPROM_STATE_NONE;
						break;

					case EEPROM_STATE_Init:
							/* Change state work operation */
							processCurrent.workstate = EEPROM_STATE_PrepareData;
						break;

					case EEPROM_STATE_PrepareData:
							/* Prepare data state work operation */
							//EEPROM_Prepare_Task();

							if(EEPROM_Prepare_Task() == EEPROM_PREPARE_Completed)
							{
								processCurrent.workstate = EEPROM_STATE_PrepareData;
								I2CProcSys.status = I2C_STATUS_IDLE;
							}
						break;

					case EEPROM_STATE_Writing_Reading:
							SysI2C_Dev_Write(SLAVE_ADDRESS, START_READ_ADDRESS, processCurrent.status.Current_buffer, EEPROM_PAGE_SIZE);

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
							processCurrent.process = EEPROM_PROCESS_UPDATE_Reading;
							processCurrent.workstate = EEPROM_STATE_NONE;
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
							//SysI2C_Dev_Read(SLAVE_ADDRESS, START_READ_ADDRESS, processCurrent.status.Backup_buffer, EEPROM_MAX_ADDR);

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
							processCurrent.status.CommandWrite = 0;
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
	if(processCurrent.status.WriteCycleTime_1s < 0xFF)
		processCurrent.status.WriteCycleTime_1s++;

	//For DEBUG
	EEPROM_Prepare_Task();
}
void EEPROM1MINTask(void)
{;}

/***********************************************************************************************************************
EEPROM prepare data task
***********************************************************************************************************************/
uint8_t EEPROM_CheckData_Task(void)
{
		uint16_t i;

		for(i = 4079; i <= 4095; i++)
		{
			if(processCurrent.status.Backup_buffer[i] != 0)
				return EEPROM_CheckData_NONE;
		}
	return EEPROM_CheckData_CaseBank;
}

uint8_t EEPROM_Prepare_Task(void)
{
	EEPROM_CheckData_Task();

	if(EEPROM_CheckData_Task() == EEPROM_CheckData_CaseBank)
	{
		processCurrent.status.Backup_buffer[StartDataWrite_location] = 1;
		processCurrent.status.Backup_buffer[StartDataAddress_location] = 1;
		processCurrent.status.Backup_buffer[StartDataCountCycle_set] = 1;
		processCurrent.status.Current_Location = StartDataAddress_location;

		return EEPROM_PREPARE_DataCaseCompleted;
	}
	else
	{
		if(processCurrent.status.Backup_buffer[4078] == 255)
		{
			memset(processCurrent.status.Backup_buffer, 0, EEPROM_MAX_ADDR - 1);

			processCurrent.status.Backup_buffer[StartDataWrite_location] = 1;
			processCurrent.status.Backup_buffer[StartDataAddress_location] = 1;
			processCurrent.status.Backup_buffer[StartDataCountCycle_set] = processCurrent.status.Backup_buffer[StartDataCountCycle_set] + 1;
			processCurrent.status.Current_Location = StartDataAddress_location;

			return EEPROM_PREPARE_DataCaseCompleted;
		}
		else if(processCurrent.status.Backup_buffer[4078] == 255 && processCurrent.status.Backup_buffer[4095] == 255)
			return EEPROM_PREPARE_Stop;
		else
		{
			if(processCurrent.status.Current_Location == 4079)	//1-255
			{
				if(processCurrent.status.Backup_buffer[4079] == 255)
				{
					processCurrent.status.Backup_buffer[4080] = 1;
					processCurrent.status.Backup_buffer[255] = processCurrent.status.Backup_buffer[255] + 1;
					processCurrent.status.Current_Location = 4080;

					return EEPROM_PREPARE_DataCaseCompleted;
				}
				else
				{
					processCurrent.status.Backup_buffer[processCurrent.status.Backup_buffer[4079]] = processCurrent.status.Backup_buffer[processCurrent.status.Backup_buffer[4079]] + 1;
					processCurrent.status.Backup_buffer[4079] = processCurrent.status.Backup_buffer[4079] + 1;
					processCurrent.status.Current_Location = 4079;

					return EEPROM_PREPARE_DataCaseCompleted;
				}
			}
			else if(processCurrent.status.Current_Location == 4080)	//256-510
			{
				if(processCurrent.status.Backup_buffer[4080] == 255)
				{
					processCurrent.status.Backup_buffer[4081] = 1;
					processCurrent.status.Backup_buffer[510] = processCurrent.status.Backup_buffer[510] + 1;
					processCurrent.status.Current_Location = 4081;

					return EEPROM_PREPARE_DataCaseCompleted;
				}
				else
				{
					processCurrent.status.Backup_buffer[processCurrent.status.Backup_buffer[4079] + processCurrent.status.Backup_buffer[4080]] = processCurrent.status.Backup_buffer[processCurrent.status.Backup_buffer[4080]] + 1;
					processCurrent.status.Backup_buffer[4080] = processCurrent.status.Backup_buffer[4080] + 1;
					processCurrent.status.Current_Location = 4080;

					return EEPROM_PREPARE_DataCaseCompleted;
				}
			}
			else if(processCurrent.status.Current_Location == 4081)	//511 - 765
			{
				if(processCurrent.status.Backup_buffer[4081] == 255)
				{
					processCurrent.status.Backup_buffer[4082] = 1;
					processCurrent.status.Backup_buffer[765] = processCurrent.status.Backup_buffer[765] + 1;
					processCurrent.status.Current_Location = 4082;

					return EEPROM_PREPARE_DataCaseCompleted;
				}
				else
				{
					processCurrent.status.Backup_buffer[processCurrent.status.Backup_buffer[4080] + processCurrent.status.Backup_buffer[4081]] = processCurrent.status.Backup_buffer[processCurrent.status.Backup_buffer[4081]] + 1;
					processCurrent.status.Backup_buffer[4081] = processCurrent.status.Backup_buffer[4081] + 1;
					processCurrent.status.Current_Location = 4081;

					return EEPROM_PREPARE_DataCaseCompleted;
				}
			}
			else if(processCurrent.status.Current_Location == 4082)	//766 - 1020
			{
				if(processCurrent.status.Backup_buffer[4082] == 255)
				{
					processCurrent.status.Backup_buffer[4083] = 1;
					processCurrent.status.Backup_buffer[1020] = processCurrent.status.Backup_buffer[1020] + 1;
					processCurrent.status.Current_Location = 4083;

					return EEPROM_PREPARE_DataCaseCompleted;
				}
				else
				{
					processCurrent.status.Backup_buffer[processCurrent.status.Backup_buffer[4081] + processCurrent.status.Backup_buffer[4082]] = processCurrent.status.Backup_buffer[processCurrent.status.Backup_buffer[4082]] + 1;
					processCurrent.status.Backup_buffer[4082] = processCurrent.status.Backup_buffer[4082] + 1;
					processCurrent.status.Current_Location = 4082;

					return EEPROM_PREPARE_DataCaseCompleted;
				}
			}
			else if(processCurrent.status.Current_Location == 4083)	//1021 - 1275
			{
				if(processCurrent.status.Backup_buffer[4083] == 255)
				{
					processCurrent.status.Backup_buffer[4084] = 1;
					processCurrent.status.Backup_buffer[1275] = processCurrent.status.Backup_buffer[1275] + 1;
					processCurrent.status.Current_Location = 4084;

					return EEPROM_PREPARE_DataCaseCompleted;
				}
				else
				{
					processCurrent.status.Backup_buffer[processCurrent.status.Backup_buffer[4082] + processCurrent.status.Backup_buffer[4083]] = processCurrent.status.Backup_buffer[processCurrent.status.Backup_buffer[4083]] + 1;
					processCurrent.status.Backup_buffer[4083] = processCurrent.status.Backup_buffer[4083] + 1;
					processCurrent.status.Current_Location = 4083;

					return EEPROM_PREPARE_DataCaseCompleted;
				}
			}
			else if(processCurrent.status.Current_Location == 4084)	//1276 - 1530
			{
				if(processCurrent.status.Backup_buffer[4084] == 255)
				{
					processCurrent.status.Backup_buffer[4085] = 1;
					processCurrent.status.Backup_buffer[1530] = processCurrent.status.Backup_buffer[1530] + 1;
					processCurrent.status.Current_Location = 4085;

					return EEPROM_PREPARE_DataCaseCompleted;
				}
				else
				{
					processCurrent.status.Backup_buffer[processCurrent.status.Backup_buffer[4083] + processCurrent.status.Backup_buffer[4084]] = processCurrent.status.Backup_buffer[processCurrent.status.Backup_buffer[4084]] + 1;
					processCurrent.status.Backup_buffer[4084] = processCurrent.status.Backup_buffer[4084] + 1;
					processCurrent.status.Current_Location = 4084;

					return EEPROM_PREPARE_DataCaseCompleted;
				}
			}
			else if(processCurrent.status.Current_Location == 4085) // 1531 - 1785
			{
					if(processCurrent.status.Backup_buffer[4085] == 255) {
						processCurrent.status.Backup_buffer[4086] = 1;
						processCurrent.status.Backup_buffer[1785] = processCurrent.status.Backup_buffer[1785] + 1;
						processCurrent.status.Current_Location = 4086;
					} else {
						processCurrent.status.Backup_buffer[processCurrent.status.Backup_buffer[4084] + processCurrent.status.Backup_buffer[4085]] = processCurrent.status.Backup_buffer[processCurrent.status.Backup_buffer[4085]] + 1;
						processCurrent.status.Backup_buffer[4085] = processCurrent.status.Backup_buffer[4085] + 1;
						processCurrent.status.Current_Location = 4085;
					}
					return EEPROM_PREPARE_DataCaseCompleted;
			}
			else if(processCurrent.status.Current_Location == 4086) // 1786 - 2040
			{
					if(processCurrent.status.Backup_buffer[4086] == 255) {
						processCurrent.status.Backup_buffer[4087] = 1;
						processCurrent.status.Backup_buffer[2040] = processCurrent.status.Backup_buffer[2040] + 1;
						processCurrent.status.Current_Location = 4087;
					} else {
						processCurrent.status.Backup_buffer[processCurrent.status.Backup_buffer[4085] + processCurrent.status.Backup_buffer[4086]] = processCurrent.status.Backup_buffer[processCurrent.status.Backup_buffer[4086]] + 1;
						processCurrent.status.Backup_buffer[4086] = processCurrent.status.Backup_buffer[4086] + 1;
						processCurrent.status.Current_Location = 4086;
					}
					return EEPROM_PREPARE_DataCaseCompleted;
			}
			else if(processCurrent.status.Current_Location == 4087) // 2041 - 2295
			{
					if(processCurrent.status.Backup_buffer[4087] == 255) {
						processCurrent.status.Backup_buffer[4088] = 1;
						processCurrent.status.Backup_buffer[2295] = processCurrent.status.Backup_buffer[2295] + 1;
						processCurrent.status.Current_Location = 4088;
					} else {
						processCurrent.status.Backup_buffer[processCurrent.status.Backup_buffer[4086] + processCurrent.status.Backup_buffer[4087]] = processCurrent.status.Backup_buffer[processCurrent.status.Backup_buffer[4087]] + 1;
						processCurrent.status.Backup_buffer[4087] = processCurrent.status.Backup_buffer[4087] + 1;
						processCurrent.status.Current_Location = 4087;
					}
					return EEPROM_PREPARE_DataCaseCompleted;
			}
			else if(processCurrent.status.Current_Location == 4088) // 2296 - 2550
			{
					if(processCurrent.status.Backup_buffer[4088] == 255) {
						processCurrent.status.Backup_buffer[4089] = 1;
						processCurrent.status.Backup_buffer[2550] = processCurrent.status.Backup_buffer[2550] + 1;
						processCurrent.status.Current_Location = 4089;
					} else {
						processCurrent.status.Backup_buffer[processCurrent.status.Backup_buffer[4087] + processCurrent.status.Backup_buffer[4088]] = processCurrent.status.Backup_buffer[processCurrent.status.Backup_buffer[4088]] + 1;
						processCurrent.status.Backup_buffer[4088] = processCurrent.status.Backup_buffer[4088] + 1;
						processCurrent.status.Current_Location = 4088;
					}
					return EEPROM_PREPARE_DataCaseCompleted;
			}
			else if(processCurrent.status.Current_Location == 4089) // 2551 - 2805
			{
					if(processCurrent.status.Backup_buffer[4089] == 255) {
						processCurrent.status.Backup_buffer[4090] = 1;
						processCurrent.status.Backup_buffer[2805] = processCurrent.status.Backup_buffer[2805] + 1;
						processCurrent.status.Current_Location = 4090;
					} else {
						processCurrent.status.Backup_buffer[processCurrent.status.Backup_buffer[4088] + processCurrent.status.Backup_buffer[4089]] = processCurrent.status.Backup_buffer[processCurrent.status.Backup_buffer[4089]] + 1;
						processCurrent.status.Backup_buffer[4089] = processCurrent.status.Backup_buffer[4089] + 1;
						processCurrent.status.Current_Location = 4089;
					}
					return EEPROM_PREPARE_DataCaseCompleted;
			}
			else if(processCurrent.status.Current_Location == 4090) // 2806 - 3060
			{
					if(processCurrent.status.Backup_buffer[4090] == 255) {
						processCurrent.status.Backup_buffer[4091] = 1;
						processCurrent.status.Backup_buffer[3060] = processCurrent.status.Backup_buffer[3060] + 1;
						processCurrent.status.Current_Location = 4091;
					} else {
						processCurrent.status.Backup_buffer[processCurrent.status.Backup_buffer[4089] + processCurrent.status.Backup_buffer[4090]] = processCurrent.status.Backup_buffer[processCurrent.status.Backup_buffer[4090]] + 1;
						processCurrent.status.Backup_buffer[4090] = processCurrent.status.Backup_buffer[4090] + 1;
						processCurrent.status.Current_Location = 4090;
					}
					return EEPROM_PREPARE_DataCaseCompleted;
			}
			else if(processCurrent.status.Current_Location == 4091) // 3061 - 3315
			{
					if(processCurrent.status.Backup_buffer[4091] == 255) {
						processCurrent.status.Backup_buffer[4092] = 1;
						processCurrent.status.Backup_buffer[3315] = processCurrent.status.Backup_buffer[3315] + 1;
						processCurrent.status.Current_Location = 4092;
					} else {
						processCurrent.status.Backup_buffer[processCurrent.status.Backup_buffer[4090] + processCurrent.status.Backup_buffer[4091]] = processCurrent.status.Backup_buffer[processCurrent.status.Backup_buffer[4091]] + 1;
						processCurrent.status.Backup_buffer[4091] = processCurrent.status.Backup_buffer[4091] + 1;
						processCurrent.status.Current_Location = 4091;
					}
					return EEPROM_PREPARE_DataCaseCompleted;
			}
			else if(processCurrent.status.Current_Location == 4092) // 3316 - 3570
			{
					if(processCurrent.status.Backup_buffer[4092] == 255) {
						processCurrent.status.Backup_buffer[4093] = 1;
						processCurrent.status.Backup_buffer[3570] = processCurrent.status.Backup_buffer[3570] + 1;
						processCurrent.status.Current_Location = 4093;
					} else {
						processCurrent.status.Backup_buffer[processCurrent.status.Backup_buffer[4091] + processCurrent.status.Backup_buffer[4092]] = processCurrent.status.Backup_buffer[processCurrent.status.Backup_buffer[4092]] + 1;
						processCurrent.status.Backup_buffer[4092] = processCurrent.status.Backup_buffer[4092] + 1;
						processCurrent.status.Current_Location = 4092;
					}
					return EEPROM_PREPARE_DataCaseCompleted;
			}
			else if(processCurrent.status.Current_Location == 4093) // 3571 - 3825
			{
					if(processCurrent.status.Backup_buffer[4093] == 255) {
						processCurrent.status.Backup_buffer[4094] = 1;
						processCurrent.status.Backup_buffer[3825] = processCurrent.status.Backup_buffer[3825] + 1;
						processCurrent.status.Current_Location = 4094;
					} else {
						processCurrent.status.Backup_buffer[processCurrent.status.Backup_buffer[4092] + processCurrent.status.Backup_buffer[4093]] = processCurrent.status.Backup_buffer[processCurrent.status.Backup_buffer[4093]] + 1;
						processCurrent.status.Backup_buffer[4093] = processCurrent.status.Backup_buffer[4093] + 1;
						processCurrent.status.Current_Location = 4093;
					}
					return EEPROM_PREPARE_DataCaseCompleted;
			}
			else if(processCurrent.status.Current_Location == 4094) // 3826 - 4080
			{
					if(processCurrent.status.Backup_buffer[4094] >= 241) {
						uint16_t j;
						for (j = 4079; j <= 4094; j++)
						{
							processCurrent.status.Backup_buffer[j] = 0;
						}
						processCurrent.status.Backup_buffer[4079] = 1;
						processCurrent.status.Backup_buffer[0] = processCurrent.status.Backup_buffer[0] + 1;
						processCurrent.status.Current_Location = 4079;
						return EEPROM_PREPARE_DataCaseCompleted;
					} else {
						processCurrent.status.Backup_buffer[processCurrent.status.Backup_buffer[4093] + processCurrent.status.Backup_buffer[4094]] = processCurrent.status.Backup_buffer[processCurrent.status.Backup_buffer[4094]] + 1;
						processCurrent.status.Backup_buffer[4094] = processCurrent.status.Backup_buffer[4094] + 1;
						processCurrent.status.Current_Location = 4094;
					}
					return EEPROM_PREPARE_DataCaseCompleted;
			}

		}
	}

	return EEPROM_PREPARE_NONE;
}
