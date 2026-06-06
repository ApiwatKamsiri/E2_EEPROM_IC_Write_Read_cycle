/***********************************************************************************************************************
* Function Name	: i2c.h
* Description  	: This function implements i2c communication function.
* author    	: Apiwat Kamsiri
* Create date 	: 29-05-2026
***********************************************************************************************************************/
/***********************************************************************************************************************
Include
***********************************************************************************************************************/
#include "i2c.h"
//Global variable
#include "r_cg_serial.h"	//I2C serial hardware define

/***********************************************************************************************************************
Define function
***********************************************************************************************************************/
SysI2C_type I2CProcSys;
static uint8_t dev_i2c_write_buffer[10]; // Buffer

/***********************************************************************************************************************
System i2c initial function
***********************************************************************************************************************/
void SysI2C_Init(void) {
	//Global variable (Start hartware)
	R_IICA0_Create();

	I2CProcSys.status = I2C_STATUS_IDLE;
	I2CProcSys.current_offset = 0;
}


/***********************************************************************************************************************
System i2c write function
***********************************************************************************************************************/
void SysI2C_Dev_Write(uint8_t slave_addr, uint16_t mem_addr, uint8_t *data, uint16_t len) {

	uint8_t i;

    if (I2CProcSys.status == I2C_STATUS_IDLE) {
			I2CProcSys.status = I2C_STATUS_BUSY_SEND;

			dev_i2c_write_buffer[0] = (uint8_t)(mem_addr & 0xFF);
			for(i = 0; i < len; i++) {
				dev_i2c_write_buffer[i+1] = data[i];
			}

			I2CProcSys.buffer = dev_i2c_write_buffer;
			I2CProcSys.length = len + 1;


			R_IICA0_Master_Send(slave_addr,dev_i2c_write_buffer, len + 1, 0);
    }
}

/***********************************************************************************************************************
System i2c Read function
***********************************************************************************************************************/
void SysI2C_Dev_Read(uint8_t slave_addr, uint16_t mem_addr, uint8_t *data, uint16_t len) {
    if (I2CProcSys.status == I2C_STATUS_IDLE) {
			I2CProcSys.status = I2C_STATUS_BUSY_SENDFORREAD;
			I2CProcSys.slave_addr = slave_addr;
			I2CProcSys.buffer = data;
			I2CProcSys.length = len;

			dev_i2c_write_buffer[0] = (uint8_t)(mem_addr & 0xFF);
			R_IICA0_Master_Send(slave_addr,dev_i2c_write_buffer, 1, 0);
    }
}

/***********************************************************************************************************************
System i2c get status function
***********************************************************************************************************************/
I2C_Status_t SysI2C_Dev_GetStatus(void) {
    return I2CProcSys.status;
}

/***********************************************************************************************************************
i2c hardware completed
***********************************************************************************************************************/
void I2C_Hardware_ISR(void) {
	if (I2CProcSys.status == I2C_STATUS_BUSY_SENDFORREAD) {
	        I2CProcSys.status = I2C_STATUS_BUSY_RECEIVE;
	        R_IICA0_Master_Receive(I2CProcSys.slave_addr, I2CProcSys.buffer, I2CProcSys.length, 0);
	    }
	    else if (I2CProcSys.status == I2C_STATUS_BUSY_RECEIVE) {
	        I2CProcSys.status = I2C_STATUS_IDLE;
	    }
	    else if (I2CProcSys.status == I2C_STATUS_BUSY_SEND) {
	        I2CProcSys.status = I2C_STATUS_IDLE;
	    }
}

/***********************************************************************************************************************
i2c hardware error
***********************************************************************************************************************/
void I2C_Hardware_Error_ISR(void) {
	I2CProcSys.status = I2C_STATUS_ERROR;
}
