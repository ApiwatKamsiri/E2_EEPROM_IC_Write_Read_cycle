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
	processCurrent.status.WriteCycleTime_20ms = 0;
	processCurrent.status.AlreadyCommandWrted = 0;
	processCurrent.status.ReadCompleted = 0;
	processCurrent.status.WriteCycle = 0;
	processCurrent.status.ReadCycle = 0;
	processCurrent.status.EEPROM_Cuurent_Address = START_READ_ADDRESS;

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
		/* Select process */
		if(processCurrent.status.CommandWrite && !processCurrent.status.CommandRead)
			processCurrent.process = EEPROM_PROCESS_UPDATE_Writing;
		else if(processCurrent.status.CommandWrite && processCurrent.status.CommandRead)
			processCurrent.process = EEPROM_PROCESS_UPDATE_Reading;

		/* Write cycle process */
		if(processCurrent.status.AlreadyCommandWrted && processCurrent.workstate == EEPROM_STATE_NONE)
		{
			if(!processCurrent.status.CommandWrite && !processCurrent.status.CommandRead)
			{
				processCurrent.status.CommandWrite = 1;
			}
			processCurrent.status.WriteCycleTime_1s = 0;
			processCurrent.status.WriteCycleTime_20ms = 0;
		}


//		if(processCurrent.status.WriteCycleTime_1s >= StartWriteTime_set && processCurrent.status.ReadCompleted)
//		{
//			processCurrent.status.AlreadyCommandWrted = 1;
//			processCurrent.status.ReadCompleted = 0;
//		}

		if(processCurrent.status.WriteCycleTime_20ms >= StartWriteTime_set && processCurrent.status.ReadCompleted)
		{
			processCurrent.status.AlreadyCommandWrted = 1;
			processCurrent.status.ReadCompleted = 0;
		}
	}


	/* I2C Get status function */
	if (SysI2C_Dev_GetStatus() == I2C_STATUS_BUSY_COMPLETED)
		processCurrent.status.GetStatusComplete = 1;
}

/***********************************************************************************************************************
EEPROM timer task
***********************************************************************************************************************/
void EEPROM1MsTask(void)
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
							if(processCurrent.process == EEPROM_PROCESS_UPDATE_Writing && processCurrent.status.AlreadyCommandWrted)
							{
								processCurrent.workstate = EEPROM_STATE_Init;
								processCurrent.status.AlreadyCommandWrted = 0;
							}
							else
								processCurrent.workstate = EEPROM_STATE_NONE;
						break;

					case EEPROM_STATE_Init:
							/* Change state work operation */
							if(EEPROM_Prepare_Task() == EEPROM_PREPARE_DataCaseCompleted)
								processCurrent.workstate = EEPROM_STATE_PrepareData;
						break;

					case EEPROM_STATE_PrepareData:
							/* Prepare data state work operation */
							if(processCurrent.status.WriteCycle)
							{
								processCurrent.workstate = EEPROM_STATE_Writing_Reading;
								processCurrent.status.WriteCycle = 0;
								I2CProcSys.status = I2C_STATUS_IDLE;
							}
							else
							{
								uint16_t i = 0;

								if(processCurrent.status.EEPROM_ALL_PAGE)
								{
									if(processCurrent.status.EEPROM_Cuurent_PAGE <= processCurrent.status.EEPROM_ALL_PAGE)
									{
										if(processCurrent.status.EEPROM_Cuurent_PAGE == 0)
											processCurrent.status.EEPROM_Cuurent_Address = START_READ_ADDRESS;
										else
											processCurrent.status.EEPROM_Cuurent_Address += EEPROM_PAGE_SIZE;

										processCurrent.status.EEPROM_Cuurent_PAGE++;
										processCurrent.status.WriteCycle = 1;
										for(i = 0; i<= EEPROM_PAGE_SIZE; i++)
										{
											processCurrent.status.Current_buffer[i] = processCurrent.status.Backup_buffer[i + processCurrent.status.EEPROM_Cuurent_Address];
										}
									}
								}
								else
									processCurrent.status.EEPROM_ALL_PAGE = EEPROM_MAX_ADDR/32;

								processCurrent.workstate = EEPROM_STATE_PrepareData;
							}
						break;
					case EEPROM_STATE_Writing_Reading:
							SysI2C_Dev_Write(SLAVE_ADDRESS, processCurrent.status.EEPROM_Cuurent_Address, processCurrent.status.Current_buffer, EEPROM_PAGE_SIZE);

							if (processCurrent.status.GetStatusComplete)
							{
								processCurrent.status.GetStatusComplete = 0;
								processCurrent.workstate = EEPROM_STATE_GetData;
								I2CProcSys.status = I2C_STATUS_NONE;
							}
						break;

					case EEPROM_STATE_GetData:
							if(processCurrent.status.EEPROM_Cuurent_PAGE <= processCurrent.status.EEPROM_ALL_PAGE - 1)
							{
								processCurrent.workstate = EEPROM_STATE_PrepareData;
							}
							else
							{
								processCurrent.workstate = EEPROM_STATE_Check;
							}
						break;

					case EEPROM_STATE_Check:
							processCurrent.workstate = EEPROM_STATE_Completed;
						break;

					case EEPROM_STATE_Completed:
							processCurrent.process = EEPROM_PROCESS_UPDATE_Reading;
							processCurrent.status.CommandRead = 1;
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
							processCurrent.status.ReadCompleted = 0;
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
							processCurrent.status.Current_Location = LastAddressAfterPowerOn();
							processCurrent.workstate = EEPROM_STATE_Check;
						break;

					case EEPROM_STATE_Check:
							processCurrent.workstate = EEPROM_STATE_Completed;
						break;

					case EEPROM_STATE_Completed:
							processCurrent.status.CommandRead = 0;
							processCurrent.status.CommandWrite = 0;
							processCurrent.status.ReadCompleted = 1;
							processCurrent.status.EEPROM_ALL_PAGE = 0;
							processCurrent.status.EEPROM_Cuurent_PAGE = 0;
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

void EEPROM10MsTask(void)
{
	;
}
void EEPROM20MsTask(void)
{
	if(processCurrent.status.WriteCycleTime_20ms < 0xFF && processCurrent.status.ReadCompleted)
		processCurrent.status.WriteCycleTime_20ms++;
}

void EEPROM1sTask(void)
{
	if(processCurrent.status.WriteCycleTime_1s < 0xFF && processCurrent.status.ReadCompleted)
		processCurrent.status.WriteCycleTime_1s++;
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
	uint16_t loc;
	uint16_t prev_sum;
	uint16_t j;
	uint16_t write_idx;

	// Case 1 : Check the data is bank
	if(EEPROM_CheckData_Task() == EEPROM_CheckData_CaseBank)
	{
		processCurrent.status.Backup_buffer[StartDataWrite_location] = 1;
		processCurrent.status.Backup_buffer[StartDataAddress_location] = 1;
		processCurrent.status.Backup_buffer[StartDataCountCycle_set] = 1;
		processCurrent.status.Current_Location = StartDataAddress_location;

		return EEPROM_PREPARE_DataCaseCompleted;
	}

	loc = processCurrent.status.Current_Location;

	// Case 2 : The data is full (Reset and set Backup buffer)
	if (processCurrent.status.Backup_buffer[4078] == 255)
	    {
	        memset(processCurrent.status.Backup_buffer, 0, EEPROM_MAX_ADDR - 1);

	        processCurrent.status.Backup_buffer[StartDataCountCycle_set]++;
	        processCurrent.status.Current_Location = StartDataAddress_location;
			processCurrent.status.Backup_buffer[StartDataAddress_location] = 1;
			processCurrent.status.Backup_buffer[StartDataWrite_location] = 1;

	        return EEPROM_PREPARE_DataCaseCompleted;
	    }

	// Case 3 : Normal operation
	if (loc >= 4079 && loc <= 4094)
	    {
			if(processCurrent.status.Backup_buffer[4094] > 253)
			{
				for(j = 4079; j <= loc; j++)
				{
					processCurrent.status.Backup_buffer[j] = 0;
				}
				processCurrent.status.Current_Location = 4079;
				write_idx = 0;
				processCurrent.status.Backup_buffer[write_idx]++;
				processCurrent.status.Backup_buffer[processCurrent.status.Current_Location] = 1;

			}
			else if (processCurrent.status.Backup_buffer[loc] >= 255)
	        {
	            prev_sum = 0;

	            for(j = 4079; j < loc; j++) {
	                prev_sum += processCurrent.status.Backup_buffer[j];
	            }
	            write_idx = prev_sum + processCurrent.status.Backup_buffer[loc];

	            processCurrent.status.Current_Location = loc + 1;
	            processCurrent.status.Backup_buffer[write_idx]++;
	            processCurrent.status.Backup_buffer[processCurrent.status.Current_Location]++;
	        }
	        else
	        {
	            prev_sum = 0;

	            for(j = 4079; j < loc; j++) {
	                prev_sum += processCurrent.status.Backup_buffer[j];
	            }
	            write_idx = prev_sum + processCurrent.status.Backup_buffer[loc];

	            processCurrent.status.Backup_buffer[write_idx]++;
	            processCurrent.status.Backup_buffer[loc]++;
	        }
	        return EEPROM_PREPARE_DataCaseCompleted;
	    }

		// Case 4 : Oters case
		memset(processCurrent.status.Backup_buffer, 0, EEPROM_MAX_ADDR);
	    return EEPROM_PREPARE_DataCaseCompleted;
}

uint16_t LastAddressAfterPowerOn(void)
{
    uint16_t addr;

    for (addr = 4079; addr <= 4094; addr++)
    {
        if (processCurrent.status.Backup_buffer[addr] < 255)
        {
            return addr;
        }
    }

    return 4079;
}
