/***********************************************************************************************************************
* Function Name	: eeprom_backup.h
* Description  	: This function implements eeprom backup function.
* author    	: Apiwat Kamsiri
* Create date 	: 29-05-2026
***********************************************************************************************************************/

#ifndef EEPROM_BACKUP_H
#define EEPROM_BACKUP_H

/***********************************************************************************************************************
Include function
***********************************************************************************************************************/
#include "..\\dev\timer.h"

#define EEPROM_SIZE_BYTES_64KB 8192
#define EEPROM_SIZE_BYTES_32KB 4096

/***********************************************************************************************************************
typedef counting function
***********************************************************************************************************************/
typedef enum {
	EEPROM_PROCESS_NONE = 0,
	EEPROM_PROCESS_UPDATE_Writing,
	EEPROM_PROCESS_UPDATE_Reading,
	EEPROM_PROCESS_UPDATE_COMPLETE,
} EepromProcessProc_em;

typedef enum {
	EEPROM_STATE_NONE = 0,
	EEPROM_STATE_Init,
	EEPROM_STATE_PrepareData,
	EEPROM_STATE_Writing_Reading,
	EEPROM_STATE_GetData,
	EEPROM_STATE_Check,
	EEPROM_STATE_Completed,
} workstate_em;

typedef struct {
	workstate_em workstate;
	uint8_t backup_buffer[EEPROM_SIZE_BYTES_64KB];
	uint8_t forceWrite;
} EepromProcess_st;

typedef struct {
	EepromProcessProc_em process;
	EepromProcess_st status;
} EepromProcessStruct_t;

/***********************************************************************************************************************
Define function
***********************************************************************************************************************/
//Global variable
extern EepromProcessStruct_t processCurrent;

void EepromBackup_Init(void);
void EEPROMBackupStateHandler(void);
void UpdateEEPROMTask(void);
void EEPROM_ReadAll_Task(void);
void EEPROM_Write_Task(void);


static void EEPROM10MsTask(void);
static void EEPROM20MsTask(void);
static void EEPROM1sTask(void);
static void EEPROM1MINTask(void);

#endif
