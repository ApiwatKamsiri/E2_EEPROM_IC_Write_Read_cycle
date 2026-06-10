/***********************************************************************************************************************
* Function Name	: i2c.h
* Description  	: This function implements i2c communication function.
* author    	: Apiwat Kamsiri
* Create date 	: 29-05-2026
***********************************************************************************************************************/
/***********************************************************************************************************************
Include
***********************************************************************************************************************/
#include <string.h>
#include "i2c.h"
//Global variable
#include "r_cg_serial.h"	//I2C serial hardware define

/***********************************************************************************************************************
Define function
***********************************************************************************************************************/
SysI2C_type I2CProcSys;
static uint8_t dev_i2c_tx_buffer[32 + 2];	// Buffer for write data from EEPROM
static uint8_t dev_i2c_tx_rx_buffer[2];		// Buffer for read data from EEPROM

/***********************************************************************************************************************
System i2c initial function
***********************************************************************************************************************/
void SysI2C_Init(void) {
	//Global variable (Start hartware)
	R_IICA0_Create();

	//Initial I2C system
	I2CProcSys.status = I2C_STATUS_NONE;
	I2CProcSys.current_offset = 0;
}


/***********************************************************************************************************************
System i2c write function
***********************************************************************************************************************/
void SysI2C_Dev_Write(uint8_t slave_addr, uint16_t mem_addr, uint8_t *data, uint16_t len) {
	//Variable data
	I2CProcSys.slave_addr = slave_addr;
	I2CProcSys.length = len;
	I2CProcSys.buffer = data;

	dev_i2c_tx_buffer[0] = (uint8_t)(mem_addr >> 8);
	dev_i2c_tx_buffer[1] = (uint8_t)(mem_addr);
	//Prepare the data
	memcpy(&dev_i2c_tx_buffer[2], I2CProcSys.buffer, I2CProcSys.length);

	R_IICA0_Master_Send(I2CProcSys.slave_addr, dev_i2c_tx_buffer, I2CProcSys.length + 2, 5);
}

/***********************************************************************************************************************
System i2c Read function
***********************************************************************************************************************/
uint8_t SysI2C_Dev_Read(uint8_t slave_addr, uint16_t mem_addr, uint8_t *data, uint16_t len)
{
	//Variable data
	I2CProcSys.slave_addr = slave_addr;
	I2CProcSys.length = len;
	I2CProcSys.buffer = data;

    dev_i2c_tx_rx_buffer[0] = (uint8_t)(mem_addr >> 8);
    dev_i2c_tx_rx_buffer[1] = (uint8_t)(mem_addr);

    //State operation
	switch (I2CProcSys.status)
	{
		case I2C_STATUS_NONE:
				//Waiting force read
				return I2C_STATUS_NONE;

		case I2C_STATUS_IDLE:
					//Change state to send address
					I2CProcSys.status = I2C_STATUS_BUSY_SENDFORREAD;
				return I2C_STATUS_BUSY_SENDFORREAD;

		case I2C_STATUS_BUSY_SENDFORREAD:
					//Start send and Change state to waiting
					I2CProcSys.status = I2C_STATUS_BUSY_WAITREADWRITE;
					R_IICA0_Master_Send(I2CProcSys.slave_addr, dev_i2c_tx_rx_buffer, 2, 5);
				return I2C_STATUS_BUSY_WAITREADWRITE;

		case I2C_STATUS_BUSY_WAITREADWRITE:
					//Waiting and change state by callback
				return I2C_STATUS_BUSY_WAITREADWRITE;

		case I2C_STATUS_BUSY_RECEIVE:
					//Start read and Change state to waiting
					I2CProcSys.status = I2C_STATUS_BUSY_WAITREAD;
					R_IICA0_Master_Receive(I2CProcSys.slave_addr, I2CProcSys.buffer, I2CProcSys.length, 5);
				return I2C_STATUS_BUSY_WAITREAD;

		case I2C_STATUS_BUSY_WAITREAD:
					//Waiting and change state by callback
				return I2C_STATUS_BUSY_WAITREAD;

		case I2C_STATUS_BUSY_COMPLETED:
					//Waiting callback send completed and reset error.
					I2CProcSys.Error_flag = I2C_ERROR_NONE;
				return I2C_STATUS_BUSY_COMPLETED;

		default :
				//Others
				return I2CProcSys.status;
	}
}

/***********************************************************************************************************************
System i2c get status function
***********************************************************************************************************************/
I2C_Status_t SysI2C_Dev_GetStatus(void)
{
	//Return status
    return I2CProcSys.status;
}

/***********************************************************************************************************************
i2c hardware completed
***********************************************************************************************************************/

void I2C_Write_Hardware_ISR(void)
{
	//State callback function write status
	switch (I2CProcSys.status)
	{
		case I2C_STATUS_BUSY_WAITREADWRITE:
				I2CProcSys.status = I2C_STATUS_BUSY_RECEIVE;
			break;

		case I2C_STATUS_BUSY_WAIT:
				I2CProcSys.status = I2C_STATUS_BUSY_COMPLETED;
			break;

		default :
			break;
	}

}

void I2C_Read_Hardware_ISR(void)
{
	//State callback function read status
	switch (I2CProcSys.status)
	{
		case I2C_STATUS_BUSY_WAITREAD:
				I2CProcSys.status = I2C_STATUS_BUSY_COMPLETED;
			break;

		default :
			break;
	}
}

/***********************************************************************************************************************
i2c hardware error
***********************************************************************************************************************/
void I2C_Hardware_Error_ISR(uint8_t flag) {

	//State callback function error status
	I2CProcSys.Error_flag = flag;

	switch (I2CProcSys.Error_flag)
	{
		case I2C_ERROR_NONE:
				// Completed
			break;

		case I2C_ERROR_BUS_BUSY:
		case I2C_ERROR_START_CON_ERR:
		case I2C_ERROR_STOP_CON_DETEC:
		case I2C_ERROR_NO_ACKNOWLEDGE:
				SysI2C_Reset_Status();	//Reset to try again
			break;

		default :
			break;
	}
}

void SysI2C_Reset_Status(void) {
	//Reset I2C when error
    I2CProcSys.status = I2C_STATUS_IDLE;
}

