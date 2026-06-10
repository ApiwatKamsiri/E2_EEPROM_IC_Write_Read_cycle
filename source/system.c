/***********************************************************************************************************************
* Function Name	: system.c
* Description  	: This function implements system function.
* author    	: Apiwat Kamsiri
* Create date 	: 29-05-2026
***********************************************************************************************************************/

/***********************************************************************************************************************
Include
***********************************************************************************************************************/
#include "system.h"

/***********************************************************************************************************************
Define function
***********************************************************************************************************************/
static System_Registry_t SysRegistryInstance;

System_Registry_t *SysProc = &SysRegistryInstance;

/***********************************************************************************************************************
Initial state
***********************************************************************************************************************/
void SystemInitState(void)
{
	SysProc->SysDev.pTimer = &TimerProcSys;
	SysProc->SysDev.pI2C = &I2CProcSys;

	SysProc->SysProcess.pEep = &processCurrent;

	//Hardware global
	R_WDT_Create();			//Initial hardware global watchdog

	//Hardware
	SysTimer_Init();		//Initial hardware timer
	SysI2C_Init();			//Initial hardware i2c

	R_PORT_Create();		//ไปเพิ่ม port หำะะรืเ ด้วย

	//Functiuon
	EepromBackup_Init();	//Initial eeprom backup

}

/***********************************************************************************************************************
System loop state
***********************************************************************************************************************/
void SystemRunState(void)
{
	//Global hardware reset watchdog
	R_WDT_Restart();

	//Global define Timer
	SysTimer_GetActiveTimer();

	//Global loop
	EEPROMBackupStateHandler();
}

/***********************************************************************************************************************
System timer loop state
***********************************************************************************************************************/
void SysTimer_1Ms(void)
{
	EEPROM1MsTask();
}

void SysTimer_10Ms(void)
{

	EEPROM10MsTask();
}

void SysTimer_20Ms(void)
{
	EEPROM20MsTask();
}

void SysTimer_100Ms(void)
{
	;
}

void SysTimer_1SEC(void)
{
	EEPROM1sTask();
}

void SysTimer_1MIN(void)
{
	EEPROM1MINTask();
}

void SysTimer_1HOUR(void)
{
	;
}

