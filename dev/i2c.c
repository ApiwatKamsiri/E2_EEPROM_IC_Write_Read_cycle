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

/***********************************************************************************************************************
Define function
***********************************************************************************************************************/
/* Timer device system variable */
SysI2C_type I2CDevProcSys;

/* Buffer I2C device system variable */
static uint8_t dev_i2c_tx_buffer[32 + 2];	// Buffer for write data from EEPROM
static uint8_t dev_i2c_tx_rx_buffer[2];		// Buffer for read data from EEPROM


/***********************************************************************************************************************
System i2c initial function
***********************************************************************************************************************/
void SysI2C_Init(void)
{
	/* Global variable (Start hartware) */
	R_IICA0_Create();

	/* Initial I2C system */
	I2CDevProcSys.status = I2C_STATUS_NONE;
	I2CDevProcSys.wait = 0xFF;
}


/***********************************************************************************************************************
System i2c write function
***********************************************************************************************************************/
uint8_t SysI2C_Dev_Write(uint8_t slave_addr, uint16_t mem_addr, uint8_t *data, uint16_t len)
{
	uint16_t i = 0;
	uint16_t send_size;

    /* State operation */
	switch (I2CDevProcSys.status)
	{
		case I2C_STATUS_NONE:
				/* Waiting force read */
				return I2C_STATUS_NONE;

		case I2C_STATUS_IDLE:
					/* Variable the data */
					I2CDevProcSys.slave_addr = slave_addr;
					I2CDevProcSys.start_addr = mem_addr;
					I2CDevProcSys.buffer = data;
					I2CDevProcSys.all_page = 0;
					I2CDevProcSys.current_page = 0;
					I2CDevProcSys.wait = 0xFF;
					I2CDevProcSys.transmitted_bytes = 0;

					/* Prepare page */
					I2CDevProcSys.all_page = len/I2C_PAGE_SIZE;

					if ((len % I2C_PAGE_SIZE) != 0)
					{
						I2CDevProcSys.all_page++;
					}

					/* Change state to send address */
					I2CDevProcSys.status = I2C_STATUS_Preparing;
				return I2C_STATUS_Preparing;

		case I2C_STATUS_Preparing:
					/* Prepare the data */
					if(I2CDevProcSys.current_page < I2CDevProcSys.all_page)
			        {
			            I2CDevProcSys.current_addr = I2CDevProcSys.start_addr + (I2CDevProcSys.current_page * I2C_PAGE_SIZE);

			            send_size = I2C_PAGE_SIZE;
			            if ((I2CDevProcSys.current_page == I2CDevProcSys.all_page - 1) && ((len % I2C_PAGE_SIZE) != 0))
			            {
			                send_size = len % I2C_PAGE_SIZE;
			            }

			            dev_i2c_tx_buffer[0] = (uint8_t)(I2CDevProcSys.current_addr >> 8);
			            dev_i2c_tx_buffer[1] = (uint8_t)(I2CDevProcSys.current_addr);

			            for(i = 0; i < send_size; i++)
			            {
			                dev_i2c_tx_buffer[2 + i] = I2CDevProcSys.buffer[I2CDevProcSys.transmitted_bytes + i];
			            }

			            I2CDevProcSys.transmitted_bytes += send_size;
			            I2CDevProcSys.current_page++;

			            I2CDevProcSys.length = send_size + 2;

			            I2CDevProcSys.status = I2C_STATUS_BUSY_SEND;
			        }
					return I2CDevProcSys.status;

		case I2C_STATUS_BUSY_SEND:
					/* Start send and Change state to waiting */
					R_IICA0_Master_Send(I2CDevProcSys.slave_addr, dev_i2c_tx_buffer, I2CDevProcSys.length, 5);

					I2CDevProcSys.status = I2C_STATUS_BUSY_WAIT;
				return I2C_STATUS_BUSY_WAIT;

		case I2C_STATUS_BUSY_WAIT:
					/* Waiting and change state by callback */
				return I2C_STATUS_BUSY_WAIT;

		case I2C_STATUS_BUSY_CHECKDATA:
					/* Check page is completed or not */
					if(I2CDevProcSys.wait >= 5)
					{
						I2CDevProcSys.wait = 0;

						if(I2CDevProcSys.current_page < I2CDevProcSys.all_page)
							I2CDevProcSys.status = I2C_STATUS_Preparing;
						else
							I2CDevProcSys.status = I2C_STATUS_BUSY_COMPLETED;
					}

				return I2CDevProcSys.status;

		case I2C_STATUS_BUSY_COMPLETED:
					/* Waiting callback send completed and reset error */
					I2CDevProcSys.Error_flag = I2C_ERROR_NONE;
				return I2C_STATUS_BUSY_COMPLETED;

		default :
				/* Others */
				return I2CDevProcSys.status;
	}
}

/***********************************************************************************************************************
System i2c Read function
***********************************************************************************************************************/
uint8_t SysI2C_Dev_Read(uint8_t slave_addr, uint16_t mem_addr, uint8_t *data, uint16_t len)
{
    //State operation
	switch (I2CDevProcSys.status)
	{
		case I2C_STATUS_NONE:
				//Waiting force read
				return I2C_STATUS_NONE;

		case I2C_STATUS_IDLE:
					/* Variable the data */
					I2CDevProcSys.slave_addr = slave_addr;
					I2CDevProcSys.start_addr = mem_addr;
					I2CDevProcSys.length = len;
					I2CDevProcSys.buffer = data;
					I2CDevProcSys.wait = 0xFF;

					/* Change state to send address */
					I2CDevProcSys.status = I2C_STATUS_Preparing;
				return I2C_STATUS_Preparing;

		case I2C_STATUS_Preparing:
					/* Prepare the data */
					dev_i2c_tx_rx_buffer[0] = (uint8_t)(I2CDevProcSys.start_addr >> 8);
					dev_i2c_tx_rx_buffer[1] = (uint8_t)(I2CDevProcSys.start_addr);

					/* Change state to send address */
					I2CDevProcSys.status = I2C_STATUS_BUSY_SENDFORREAD;
				return I2C_STATUS_BUSY_SENDFORREAD;

		case I2C_STATUS_BUSY_SENDFORREAD:
					/* Start send and Change state to waiting */
					R_IICA0_Master_Send(I2CDevProcSys.slave_addr, dev_i2c_tx_rx_buffer, sizeof(dev_i2c_tx_rx_buffer), 5);

					/* Change state to send address */
					I2CDevProcSys.status = I2C_STATUS_BUSY_WAITREADWRITE;
				return I2C_STATUS_BUSY_WAITREADWRITE;

		case I2C_STATUS_BUSY_WAITREADWRITE:
					/* Waiting and change state by callback */
				return I2C_STATUS_BUSY_WAITREADWRITE;

		case I2C_STATUS_BUSY_RECEIVE:
					/* Start read and Change state to waiting */
					R_IICA0_Master_Receive(I2CDevProcSys.slave_addr, I2CDevProcSys.buffer, I2CDevProcSys.length, 5);

					/* Change state to send address */
					I2CDevProcSys.status = I2C_STATUS_BUSY_WAITREAD;
				return I2C_STATUS_BUSY_WAITREAD;

		case I2C_STATUS_BUSY_WAITREAD:
					/* Waiting and change state by callback */
				return I2C_STATUS_BUSY_WAITREAD;

		case I2C_STATUS_BUSY_CHECKDATA:
					/* Check data */
					I2CDevProcSys.status = I2C_STATUS_BUSY_COMPLETED;
				return I2C_STATUS_BUSY_COMPLETED;

		case I2C_STATUS_BUSY_COMPLETED:
					/* Waiting callback send completed and reset error */
					I2CDevProcSys.Error_flag = I2C_ERROR_NONE;
				return I2C_STATUS_BUSY_COMPLETED;

		default :
				/* Others */
				return I2CDevProcSys.status;
	}
}


/***********************************************************************************************************************
System i2c get status function
***********************************************************************************************************************/
I2C_Status_t SysI2C_Dev_GetStatus(void)
{
	/* Return status */
    return I2CDevProcSys.status;
}


/***********************************************************************************************************************
i2c hardware completed
***********************************************************************************************************************/
void I2C_Write_Hardware_ISR(void)
{
	/* State callback function write status */
	switch (I2CDevProcSys.status)
	{
		case I2C_STATUS_BUSY_WAITREADWRITE:
				I2CDevProcSys.status = I2C_STATUS_BUSY_RECEIVE;
			break;

		case I2C_STATUS_BUSY_WAIT:
				R_IICA0_StopCondition();	//Stop condition for update to EEPROM IC and completed
				I2CDevProcSys.wait = 0;
				I2CDevProcSys.status = I2C_STATUS_BUSY_CHECKDATA;
			break;

		default :
			break;
	}
}

void I2C_Read_Hardware_ISR(void)
{
	/* State callback function read status */
	switch (I2CDevProcSys.status)
	{
		case I2C_STATUS_BUSY_WAITREAD:
				//R_IICA0_StopCondition();	//Stop condition for completed
				I2CDevProcSys.status = I2C_STATUS_BUSY_CHECKDATA;
			break;

		default :
			break;
	}
}


/***********************************************************************************************************************
i2c hardware error
***********************************************************************************************************************/
void I2C_Hardware_Error_ISR(uint8_t flag) {

	/* State callback function error status */
	I2CDevProcSys.Error_flag = (I2C_Error_t)flag;

	switch (I2CDevProcSys.Error_flag)
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

void SysI2C_Reset_Status(void)
{
	/* Reset I2C when error */
    I2CDevProcSys.status = I2C_STATUS_IDLE;
}

void SysI2C1MsTask(void)
{
	if(I2CDevProcSys.wait < 0xFF)
		I2CDevProcSys.wait++;
}

