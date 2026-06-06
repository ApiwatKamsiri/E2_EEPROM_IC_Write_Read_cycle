/***********************************************************************************************************************
* Function Name	: i2c.h
* Description  	: This function implements i2c communication function.
* author    	: Apiwat Kamsiri
* Create date 	: 29-05-2026
***********************************************************************************************************************/
#ifndef I2C_H
#define I2C_H

/***********************************************************************************************************************
Include function
***********************************************************************************************************************/
#include "r_cg_macrodriver.h"


/***********************************************************************************************************************
typedef counting function
***********************************************************************************************************************/
typedef enum {
    I2C_STATUS_IDLE,
	I2C_STATUS_BUSY_SEND,
    I2C_STATUS_BUSY_SENDFORREAD,
	I2C_STATUS_BUSY_RECEIVE,
    I2C_STATUS_ERROR
} I2C_Status_t;

typedef struct
{
	I2C_Status_t status;
	uint8_t      *buffer;
	uint8_t      slave_addr;
	uint16_t     length;
	uint16_t     current_offset;
} SysI2C_type;


/***********************************************************************************************************************
Define function
***********************************************************************************************************************/
//Global variable
extern SysI2C_type I2CProcSys;

extern void SysI2C_Init(void);
extern void SysI2C_Dev(void);
extern void I2C_Hardware_ISR(void);
extern void I2C_Hardware_Error_ISR(void);
extern void SysI2C_Dev_Write(uint8_t slave_addr, uint16_t mem_addr, uint8_t *data, uint16_t len);
extern void SysI2C_Dev_Read(uint8_t slave_addr, uint16_t mem_addr, uint8_t *data, uint16_t len);
extern I2C_Status_t SysI2C_Dev_GetStatus(void);


#endif
