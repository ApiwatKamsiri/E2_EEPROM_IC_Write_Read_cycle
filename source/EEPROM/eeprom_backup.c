/***********************************************************************************************************************
* Function Name	: eeprom_backup.c
* Description  	: This function implements eeprom backup function.
* author    	: Apiwat Kamsiri
* Create date 	: 29-05-2026
***********************************************************************************************************************/

/***********************************************************************************************************************
Include
***********************************************************************************************************************/
#include "eeprom_backup.h"

/***********************************************************************************************************************
Define function
***********************************************************************************************************************/
/* EEPROM process system variable */
EepromProcessStruct_t EepProcSys;

/***********************************************************************************************************************
Initial EEPROM backup function
***********************************************************************************************************************/
void EepromBackup_Init(void)
{
	/* Initial state */
	EepProcSys.process = EEPROM_PROCESS_NONE;
	EepProcSys.workstate = EEPROM_STATE_Init;
	EepProcSys.status.FirstPlug = 1;
	EepProcSys.status.WriteCycleTime_1s = 0;
	EepProcSys.status.WriteCycleTime_20ms = 0;
	EepProcSys.status.AlreadyCommandWrted = 0;
	EepProcSys.status.ReadCompleted = 0;
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
	if(EepProcSys.status.FirstPlug)
	{
		EepProcSys.process = EEPROM_PROCESS_UPDATE_Reading;
		EepProcSys.status.CommandRead = 1;
		EepProcSys.status.FirstPlug = 0;
	}
	else
	{
		/* Select process */
		if(EepProcSys.status.CommandWrite && !EepProcSys.status.CommandRead)
			EepProcSys.process = EEPROM_PROCESS_UPDATE_Writing;
		else if(EepProcSys.status.CommandWrite && EepProcSys.status.CommandRead)
			EepProcSys.process = EEPROM_PROCESS_UPDATE_Reading;

		/* Write cycle process */
		if(EepProcSys.status.AlreadyCommandWrted && EepProcSys.workstate == EEPROM_STATE_NONE)
		{
			if(!EepProcSys.status.CommandWrite && !EepProcSys.status.CommandRead)
			{
				EepProcSys.status.CommandWrite = 1;
			}
			EepProcSys.status.WriteCycleTime_1s = 0;
			EepProcSys.status.WriteCycleTime_20ms = 0;
		}


//		if(EepProcSys.status.WriteCycleTime_1s >= StartWriteTime_set && EepProcSys.status.ReadCompleted)
//		{
//			EepProcSys.status.AlreadyCommandWrted = 1;
//			EepProcSys.status.ReadCompleted = 0;
//		}

		if(EepProcSys.status.WriteCycleTime_20ms >= StartWriteTime_set && EepProcSys.status.ReadCompleted)
		{
			EepProcSys.status.AlreadyCommandWrted = 1;
			EepProcSys.status.ReadCompleted = 0;
		}
	}
}

/***********************************************************************************************************************
EEPROM timer task
***********************************************************************************************************************/
void EEPROM1MsTask(void)
{
	switch (EepProcSys.process)
	{
		case EEPROM_PROCESS_NONE:
				EepProcSys.workstate = EEPROM_STATE_NONE;
			break;

		case EEPROM_PROCESS_UPDATE_Writing:
				/* Write process operation */
				switch (EepProcSys.workstate)
				{
					case EEPROM_STATE_NONE:
							/* Change state work operation */
							if(EepProcSys.process == EEPROM_PROCESS_UPDATE_Writing && EepProcSys.status.AlreadyCommandWrted)
							{
								EepProcSys.workstate = EEPROM_STATE_Init;
								EepProcSys.status.AlreadyCommandWrted = 0;
							}
							else
								EepProcSys.workstate = EEPROM_STATE_NONE;
						break;

					case EEPROM_STATE_Init:
							/* Change state work operation */
							if(EEPROM_Prepare_Task() == EEPROM_PREPARE_DataCaseCompleted)
								EepProcSys.workstate = EEPROM_STATE_PrepareData;
						break;

					case EEPROM_STATE_PrepareData:
							/* Prepare data state work operation */
							EepProcSys.workstate = EEPROM_STATE_Writing_Reading;
							I2CDevProcSys.status = I2C_STATUS_IDLE;

						break;
					case EEPROM_STATE_Writing_Reading:
							/* Send write state work operation */
							if(SysI2C_Dev_Write(SLAVE_ADDRESS, START_READ_ADDRESS, EepProcSys.status.Backup_buffer, EEPROM_MAX_ADDR) == I2C_STATUS_BUSY_COMPLETED)
							{
								EepProcSys.workstate = EEPROM_STATE_GetData;
								I2CDevProcSys.status = I2C_STATUS_NONE;
							}
						break;

					case EEPROM_STATE_GetData:
							/* Get data state work operation */
							EepProcSys.workstate = EEPROM_STATE_Check;
						break;

					case EEPROM_STATE_Check:
							/* Check data state work operation */
							EepProcSys.workstate = EEPROM_STATE_Completed;
						break;

					case EEPROM_STATE_Completed:
							/* Complete write data state work operation */
							EepProcSys.process = EEPROM_PROCESS_UPDATE_Reading;
							EepProcSys.status.CommandRead = 1;
							EepProcSys.workstate = EEPROM_STATE_NONE;
						break;

					default:
						break;
				}
			break;

		case EEPROM_PROCESS_UPDATE_Reading:
				/* Read process operation */
				switch (EepProcSys.workstate)
				{
					case EEPROM_STATE_NONE:
							/* Change state work operation */
							if(EepProcSys.process == EEPROM_PROCESS_UPDATE_Reading)
								EepProcSys.workstate = EEPROM_STATE_Init;
							else
								EepProcSys.workstate = EEPROM_STATE_NONE;
						break;

					case EEPROM_STATE_Init:
							/* Change state work operation */
							EepProcSys.workstate = EEPROM_STATE_PrepareData;
							EepProcSys.status.ReadCompleted = 0;
							I2CDevProcSys.status = I2C_STATUS_IDLE;
						break;

					case EEPROM_STATE_PrepareData:
							/* Prepare data state work operation */
							EepProcSys.workstate = EEPROM_STATE_Writing_Reading;
						break;

					case EEPROM_STATE_Writing_Reading:
							/* Send Read state work operation */
							if(SysI2C_Dev_Read(SLAVE_ADDRESS, START_READ_ADDRESS, EepProcSys.status.Backup_buffer, EEPROM_MAX_ADDR) == I2C_STATUS_BUSY_COMPLETED)
							{
								EepProcSys.workstate = EEPROM_STATE_GetData;
								I2CDevProcSys.status = I2C_STATUS_NONE;
							}
						break;

					case EEPROM_STATE_GetData:
							/* Get data state work operation */
							EepProcSys.status.Current_Location = LastAddressAfterPowerOn();
							EepProcSys.workstate = EEPROM_STATE_Check;
						break;

					case EEPROM_STATE_Check:
							/* Check data state work operation */
							EepProcSys.workstate = EEPROM_STATE_Completed;
						break;

					case EEPROM_STATE_Completed:
							/* Complete write data state work operation */
							EepProcSys.status.CommandRead = 0;
							EepProcSys.status.CommandWrite = 0;
							EepProcSys.status.ReadCompleted = 1;
							EepProcSys.process = EEPROM_PROCESS_UPDATE_COMPLETE;
						break;

					default:
						break;
				}
			break;

		case EEPROM_PROCESS_UPDATE_COMPLETE:
				/* Complete write data process work operation */
				EepProcSys.process = EEPROM_PROCESS_NONE;
			break;

		default:
			break;
	}
}


void EEPROM10MsTask(void)
{;}


void EEPROM20MsTask(void)
{
	if(EepProcSys.status.WriteCycleTime_20ms < 0xFF && EepProcSys.status.ReadCompleted)
		EepProcSys.status.WriteCycleTime_20ms++;
}


void EEPROM1sTask(void)
{
	if(EepProcSys.status.WriteCycleTime_1s < 0xFF && EepProcSys.status.ReadCompleted)
		EepProcSys.status.WriteCycleTime_1s++;
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
			if(EepProcSys.status.Backup_buffer[i] != 0)
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
		EepProcSys.status.Backup_buffer[StartDataWrite_location] = 1;
		EepProcSys.status.Backup_buffer[StartDataAddress_location] = 1;
		EepProcSys.status.Backup_buffer[StartDataCountCycle_set] = 1;
		EepProcSys.status.Current_Location = StartDataAddress_location;

		return EEPROM_PREPARE_DataCaseCompleted;
	}

	loc = EepProcSys.status.Current_Location;

	// Case 2 : The data is full (Reset and set Backup buffer)
	if (EepProcSys.status.Backup_buffer[4078] == 255)
	    {
	        memset(EepProcSys.status.Backup_buffer, 0, EEPROM_MAX_ADDR - 1);

	        EepProcSys.status.Backup_buffer[StartDataCountCycle_set]++;
	        EepProcSys.status.Current_Location = StartDataAddress_location;
			EepProcSys.status.Backup_buffer[StartDataAddress_location] = 1;
			EepProcSys.status.Backup_buffer[StartDataWrite_location] = 1;

	        return EEPROM_PREPARE_DataCaseCompleted;
	    }

	// Case 3 : Normal operation
	if (loc >= 4079 && loc <= 4094)
	    {
			if(EepProcSys.status.Backup_buffer[4094] > 253)
			{
				for(j = 4079; j <= loc; j++)
				{
					EepProcSys.status.Backup_buffer[j] = 0;
				}
				EepProcSys.status.Current_Location = 4079;
				write_idx = 0;
				EepProcSys.status.Backup_buffer[write_idx]++;
				EepProcSys.status.Backup_buffer[EepProcSys.status.Current_Location] = 1;

			}
			else if (EepProcSys.status.Backup_buffer[loc] >= 255)
	        {
	            prev_sum = 0;

	            for(j = 4079; j < loc; j++) {
	                prev_sum += EepProcSys.status.Backup_buffer[j];
	            }
	            write_idx = prev_sum + EepProcSys.status.Backup_buffer[loc];

	            EepProcSys.status.Current_Location = loc + 1;
	            EepProcSys.status.Backup_buffer[write_idx]++;
	            EepProcSys.status.Backup_buffer[EepProcSys.status.Current_Location]++;
	        }
	        else
	        {
	            prev_sum = 0;

	            for(j = 4079; j < loc; j++) {
	                prev_sum += EepProcSys.status.Backup_buffer[j];
	            }
	            write_idx = prev_sum + EepProcSys.status.Backup_buffer[loc];

	            EepProcSys.status.Backup_buffer[write_idx]++;
	            EepProcSys.status.Backup_buffer[loc]++;
	        }
	        return EEPROM_PREPARE_DataCaseCompleted;
	    }

		// Case 4 : Oters case
		memset(EepProcSys.status.Backup_buffer, 0, EEPROM_MAX_ADDR);
	    return EEPROM_PREPARE_DataCaseCompleted;
}


uint16_t LastAddressAfterPowerOn(void)
{
    uint16_t addr;

    for (addr = 4079; addr <= 4094; addr++)
    {
        if (EepProcSys.status.Backup_buffer[addr] < 255)
        {
            return addr;
        }
    }

    return 4079;
}
