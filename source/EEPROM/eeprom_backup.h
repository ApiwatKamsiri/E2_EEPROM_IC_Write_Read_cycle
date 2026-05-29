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


/***********************************************************************************************************************
typedef counting function
***********************************************************************************************************************/
typedef enum {
    EEPROM_PROCESS_NONE = 0,
	EEPROM_PROCESS_PERIODIC_10MS,
    EEPROM_PROCESS_PERIODIC_20MS,
    EEPROM_PROCESS_PERIODIC_1SEC,
    EEPROM_PROCESS_PERIODIC_1MIN,
    EEPROM_PROCESS_UPDATE_COMPLETE
} EepromProcess_1ms;

typedef struct {
	EepromProcess_1ms process;
} EepromProcessStruct_t;

/***********************************************************************************************************************
Define function
***********************************************************************************************************************/
void EepromBackup_Init(void);
void EEPROMBackupStateHandler(void);
void EEPROMBackupStateDefault(SysTimer_Channel_1ms activeTimer);

static void EEPROM10MsTask(void);
static void EEPROM20MsTask(void);
static void EEPROM1sTask(void);
static void EEPROM1MINTask(void);


#endif
