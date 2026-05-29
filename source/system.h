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
#include "EEPROM\eeprom_backup.h"
//Global variable
#include "r_cg_macrodriver.h"



/***********************************************************************************************************************
Define function
***********************************************************************************************************************/
#define CoreSoftwareStock 92740654
#define CoreSoftwareVersion 00
#define ParameterSoftwareVersion 00

extern void SystemRunTimerStateDefault(SysTimer_Channel_1ms activeTimer);
extern void SystemInitState(void);
extern void SystemRunTimerState(void);
extern void MainState(void);

#endif
