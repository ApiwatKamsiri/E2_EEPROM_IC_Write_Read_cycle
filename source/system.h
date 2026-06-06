/***********************************************************************************************************************
* Function Name	: system.h
* Description  	: This function implements system function.
* author    	: Apiwat Kamsiri
* Create date 	: 29-05-2026
***********************************************************************************************************************/

#ifndef SYSTEM_H
#define SYSTEM_H

/***********************************************************************************************************************
Include function
***********************************************************************************************************************/
#include "..\\dev\timer.h"
#include "..\\dev\i2c.h"
#include "EEPROM\eeprom_backup.h"
//Global variable
#include "r_cg_macrodriver.h"

/***********************************************************************************************************************
Typedef struc function
***********************************************************************************************************************/
typedef struct {
	SysTimer_type *pTimer;
	SysI2C_type *pI2C;
} SysDev_type;

typedef struct {
	EepromProcessStruct_t *pEep;
} SysProcess_type;

typedef struct {
    SysDev_type SysDev;
    SysProcess_type SysProcess;
} System_Registry_t;

/***********************************************************************************************************************
Define function
***********************************************************************************************************************/
#define CoreSoftwareStock 92740654
#define CoreSoftwareVersion 00
#define ParameterSoftwareVersion 00

extern void SystemInitState(void);
extern void SystemRunTimerState(void);
extern void MainState(void);

extern System_Registry_t *SysProc;

#endif
