/*
 * ADS1220.h
 *
 *  Created on: Oct 27, 2026
 *      Author: ACER
 */

#ifndef INC_ADS1220_H_
#define INC_ADS1220_H_
#include "stm32f4xx_hal.h"
#include <stdint.h>

/* Chip Select (CS) - PB6: Active-low signal used to select the ADS1220 for SPI communication (Datasheet Section 8.5.1.1) */
#define ADS1220_CS_GPIO_Port    GPIOB    /*  We connected the ADS1220 module's CS wire to Port B of the STM32F446RE.*/
#define ADS1220_CS_Pin          GPIO_PIN_6    /* Within Port B, we selected pin 6, which is called PB6.*/

/* Data Ready (DRDY) - PB5: Indicates when new ADC conversion data are ready to be retrieved (Datasheet Section 8.5.1.3).*/
#define ADS1220_DRDY_GPIO_Port  GPIOB      /* We connected the ADS1220 module's DRDY wire to Port B of the STM32F446RE. */
#define ADS1220_DRDY_Pin        GPIO_PIN_5      /* Within Port B, we selected pin 5, which is called PB5. */

/* Commands */
#define ADS1220_CMD_RESET      0x06U   /* Resets the ADS1220 to its default configuration (Datasheet Section 8.5.3.1). */
#define ADS1220_CMD_START      0x08U   /* Starts a single conversion or starts/restarts continuous conversions (Datasheet Section 8.5.3.2). */
#define ADS1220_CMD_POWERDOWN  0x02U   /* Places the ADS1220 into power-down mode while retaining register values (Datasheet Section 8.5.3.3). */
#define ADS1220_CMD_RDATA      0x10U   /* Loads the latest conversion result for retrieval through SPI (Datasheet Section 8.5.3.4). */
#define ADS1220_CMD_WREG_4     0x43U   /* Writes four configuration bytes starting at register 0 (Datasheet Section 8.5.3.6). */

/* Voltage calculation */
#define ADS1220_VREF_MV        2048LL    /* Internal reference voltage of 2.048 V converted to 2048 mV (Datasheet Section 8.3.3, Voltage Reference). */
#define ADS1220_FULL_SCALE     8388608LL    /* 2^23 scaling factor derived from the 24-bit output format (Datasheet Section 8.5.2, Equation 15). */

/* Function declarations */
HAL_StatusTypeDef ADS1220_Init(void);				/* Initializes the ADS1220. */
HAL_StatusTypeDef ADS1220_ReadRaw(int32_t *raw);	/* Reads the raw ADC conversion result. */
HAL_StatusTypeDef ADS1220_ReadVoltage(int32_t *raw,int32_t *voltage_mV);  	  /* Reads the raw ADC result and calculates voltage in mV. */

#endif /* INC_ADS1220_H_ */
