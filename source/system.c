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
/* Process system variable */
System_Registry_t SysProc;


/***********************************************************************************************************************
System software version control state
***********************************************************************************************************************/
void SystemSoftwareVersionControl(void)
{
	/* Stock software control system */
	SysProc.StockControl.Stock[0] = (((CoreSoftwareStock / 10000) / 10000) % 10000);
	SysProc.StockControl.Stock[1] = ((CoreSoftwareStock / 10000) % 10000);
	SysProc.StockControl.Stock[2] = (CoreSoftwareStock % 10000);

	/* Core and Version software control system */
	SysProc.StockControl.ParameterVersion = ParameterSoftwareVersion;
	SysProc.StockControl.CoreVersion = CoreSoftwareVersion;
}

/***********************************************************************************************************************
Initial state
***********************************************************************************************************************/
void SystemInitState(void)
{
	/* Pointer system device process variable */
	SysProc.SysDev.devTimer = &TimerDevProcSys;
	SysProc.SysDev.devI2C = &I2CDevProcSys;

	/* Pointer system process variable */
	SysProc.SysProcess.processEep = &EepProcSys;

	/* Global hardware initial */
	R_WDT_Create();			//Initial hardware global watchdog
	R_PORT_Create();		//Initial hardware port

	/* Hardware initial */
	SysTimer_Init();		//Initial hardware timer
	SysI2C_Init();			//Initial hardware i2c

	/* Functiuon initial */
	EepromBackup_Init();	//Initial eeprom backup

	/* Stock and Version software control */
	SystemSoftwareVersionControl();

}

/***********************************************************************************************************************
System loop state
***********************************************************************************************************************/
void SystemRunState(void)
{
	/* Global hardware state */
	R_WDT_Restart();				//Watch dog restart

	/* Device state */
	SysTimer_GetActiveTimer();		//Timer dev state

	/* Process state */
	EEPROMBackupStateHandler();		//Eeprom process state

}

/***********************************************************************************************************************
System timer loop state
***********************************************************************************************************************/
void SysTimer_1Ms(void)
{
	/* Device timer state */
	SysI2C1MsTask();

	/* Process timer state */
	EEPROM1MsTask();
}

void SysTimer_10Ms(void)
{
	/* Process timer state */
	EEPROM10MsTask();
}

void SysTimer_20Ms(void)
{
	/* Process timer state */
	EEPROM20MsTask();
}

void SysTimer_100Ms(void)
{
	/* Process timer state */
	;
}

void SysTimer_1SEC(void)
{
	/* Process timer state */
	EEPROM1sTask();
}

void SysTimer_1MIN(void)
{
	/* Process timer state */
	EEPROM1MINTask();
}

void SysTimer_1HOUR(void)
{
	/* Process timer state */
	;
}

