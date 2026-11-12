/***********************************************************************************************************************
* Function Name	: eeprom_config.h
* Description  	: This function implements eeprom backup function.
* author    	: Apiwat Kamsiri
* Create date 	: 29-05-2026
***********************************************************************************************************************/

#ifndef EEPROM_CONFIG_H
#define EEPROM_CONFIG_H

/***********************************************************************************************************************
Define function
***********************************************************************************************************************/

#define EEPROM_2kbit    	2u
#define EEPROM_4kbit    	4u
#define EEPROM_8kbit    	8u
#define EEPROM_16kbit    	16u
#define EEPROM_32kbit    	32u
#define EEPROM_64kbit    	64u
#define EEPROM_128kbit   	128u
#define EEPROM_256kbit   	256u

// Select the EERPROM type
#define EEPROM_TYPE     EEPROM_16kbit


#if   (EEPROM_TYPE == EEPROM_2kbit)
  #define EEPROM_SIZE_BYTES   256u
  #define EEPROM_PAGE_SIZE    8u
  #define EEPROM_ADDR_BYTES   1u
#elif (EEPROM_TYPE == EEPROM_4kbit)
  #define EEPROM_SIZE_BYTES   512u
  #define EEPROM_PAGE_SIZE    16u
  #define EEPROM_ADDR_BYTES   1u
#elif (EEPROM_TYPE == EEPROM_8kbit)
  #define EEPROM_SIZE_BYTES   1024u
  #define EEPROM_PAGE_SIZE    16u
  #define EEPROM_ADDR_BYTES   1u
#elif (EEPROM_TYPE == EEPROM_16kbit)
  #define EEPROM_SIZE_BYTES   2048u
  #define EEPROM_PAGE_SIZE    16u
  #define EEPROM_ADDR_BYTES   1u
#elif (EEPROM_TYPE == EEPROM_32kbit)
  #define EEPROM_SIZE_BYTES   4096u
  #define EEPROM_PAGE_SIZE    32u
  #define EEPROM_ADDR_BYTES   2u
#elif (EEPROM_TYPE == EEPROM_64kbit)
  #define EEPROM_SIZE_BYTES   8192u
  #define EEPROM_PAGE_SIZE    32u
  #define EEPROM_ADDR_BYTES   2u
#elif (EEPROM_TYPE == EEPROM_128kbit)
  #define EEPROM_SIZE_BYTES   16384u
  #define EEPROM_PAGE_SIZE    64u
  #define EEPROM_ADDR_BYTES   2u
#elif (EEPROM_TYPE == EEPROM_256kbit)
  #define EEPROM_SIZE_BYTES   32768u
  #define EEPROM_PAGE_SIZE    64u
  #define EEPROM_ADDR_BYTES   2u
#else
  #error "EEPROM_TYPE not supported"
#endif

#define EEPROM_SLAVE_BASE       0xA0u	// Slave address device
#define EEPROM_WRITE_CYCLE_MS   6u      // Cycle write


#endif