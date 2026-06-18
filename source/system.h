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
/* Global variable */
#include <string.h>
#include "r_cg_macrodriver.h"
#include "r_cg_cgc.h"
#include "r_cg_port.h"
#include "r_cg_serial.h"
#include "r_cg_timer.h"
#include "r_cg_wdt.h"
#include "r_cg_userdefine.h"

/* Variable */
#include "..\\dev\timer.h"
#include "..\\dev\i2c.h"
#include "EEPROM\eeprom_backup.h"


/***********************************************************************************************************************
Typedef struc function
***********************************************************************************************************************/
typedef struct {
	uint16_t Stock[3];
	uint8_t ParameterVersion;
	uint8_t CoreVersion;
} SysStockandVersion_Control;

typedef struct {
	SysTimer_type *devTimer;
	SysI2C_type *devI2C;
} SysDev_type;

typedef struct {
	EepromProcessStruct_t *processEep;
} SysProcess_type;

typedef struct {
	SysStockandVersion_Control StockControl;
    SysDev_type SysDev;
    SysProcess_type SysProcess;
} System_Registry_t;


/***********************************************************************************************************************
Define function
***********************************************************************************************************************/
/* Process system variable */
extern System_Registry_t SysProc;

#define CoreSoftwareStock 92740654
#define CoreSoftwareVersion 02
#define ParameterSoftwareVersion 00

extern void SystemInitState(void);
extern void SystemRunState(void);
extern void SystemSoftwareVersionControl(void);


#endif
