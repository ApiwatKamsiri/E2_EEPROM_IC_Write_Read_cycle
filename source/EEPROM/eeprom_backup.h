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

#define SLAVE_ADDRESS 160							//160dec -> 0xA0
#define EEPROM_PAGE_SIZE 32							//Write cycle
#define START_READ_ADDRESS 0						//Start address for read
#define EEPROM_SIZE_BYTES_64KB 8192					//EEPROM size 64KB
#define EEPROM_SIZE_BYTES_32KB 4096					//EEPROM size 32KB
//#define SELECT64KB									//Define when use 64KBIT

#ifdef SELECT64KB
#define EEPROM_MAX_ADDR EEPROM_SIZE_BYTES_64KB
#else
#define EEPROM_MAX_ADDR EEPROM_SIZE_BYTES_32KB
#endif

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

typedef enum {
	EEPROM_PREPARE_NONE = 0,
	EEPROM_PREPARE_DataCaseCompleted,
	EEPROM_PREPARE_Stop,
} resultPrepareData_em;

typedef enum {
	EEPROM_CheckData_NONE = 0,
	EEPROM_CheckData_CaseBank,
} resultCheckData_em;

typedef struct {
	uint8_t FirstPlug :1;
	uint8_t CommandWrite :1;
	uint8_t CommandRead :1;
	uint8_t GetStatusComplete :1;
	uint8_t AlreadyCommandWrted :1;
	resultCheckData_em Get_ResultCheckData;
	resultPrepareData_em Get_ResultPrepareData;
	uint8_t WriteCycleTime_1s;
	uint16_t Current_Location;
	uint8_t EEPROM_ALL_PAGE;
	uint8_t EEPROM_Cuurent_PAGE;
	uint16_t EEPROM_Cuurent_Address;
	uint8_t Current_buffer[EEPROM_PAGE_SIZE];
	uint8_t Backup_buffer[EEPROM_MAX_ADDR];
} EepromProcess_st;

typedef struct {
	EepromProcessProc_em process;
	workstate_em workstate;
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
uint8_t EEPROM_CheckData_Task(void);
uint8_t EEPROM_Prepare_Task(void);

extern void EEPROM1MsTask(void);
extern void EEPROM10MsTask(void);
extern void EEPROM20MsTask(void);
extern void EEPROM1sTask(void);
extern void EEPROM1MINTask(void);

#endif
