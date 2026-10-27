/*
 * ADS1220.c
 *
 *  Created on: Oct 27, 2026
 *      Author: ACER
 */
#include "ADS1220.h"
#include <stddef.h>

extern SPI_HandleTypeDef hspi1;

/* Send one command to ADS1220 */
static HAL_StatusTypeDef ADS1220_SendCommand(uint8_t command)
{
    HAL_StatusTypeDef status;

    HAL_GPIO_WritePin(ADS1220_CS_GPIO_Port,ADS1220_CS_Pin, GPIO_PIN_RESET);

    status = HAL_SPI_Transmit(&hspi1, &command, 1U, 100U);

    HAL_GPIO_WritePin(ADS1220_CS_GPIO_Port,ADS1220_CS_Pin, GPIO_PIN_SET);

    return status;
}

/* Initialize ADS1220 */
HAL_StatusTypeDef ADS1220_Init(void)
{
    HAL_StatusTypeDef status;

    uint8_t config[5] = {
        ADS1220_CMD_WREG_4, 0x81U, 0x04U, 0x00U,  0x00U
		/* Register 0: AIN0, gain 1, PGA bypass */
		/* Register 1: 20 SPS, single-shot mode */
		/* Register 2: internal reference */
		/* Register 3: default settings */
    };

    HAL_GPIO_WritePin(ADS1220_CS_GPIO_Port,ADS1220_CS_Pin, GPIO_PIN_SET);

    HAL_Delay(10U);

    status = ADS1220_SendCommand(ADS1220_CMD_RESET);
    if (status != HAL_OK){
        return status;
    }

    HAL_Delay(2U);

    HAL_GPIO_WritePin(ADS1220_CS_GPIO_Port,ADS1220_CS_Pin, GPIO_PIN_RESET);

    status = HAL_SPI_Transmit(&hspi1, config,sizeof(config), 100U);

    HAL_GPIO_WritePin(ADS1220_CS_GPIO_Port,ADS1220_CS_Pin, GPIO_PIN_SET);

    return status;
}

/* Wait for DRDY to indicate conversion completion */
static HAL_StatusTypeDef ADS1220_WaitReady(uint32_t timeout_ms)
{
    uint32_t start = HAL_GetTick();

    while (HAL_GPIO_ReadPin(ADS1220_DRDY_GPIO_Port,ADS1220_DRDY_Pin) == GPIO_PIN_SET)
    {
        if ((HAL_GetTick() - start) >=timeout_ms)
            return HAL_TIMEOUT;
    }

    return HAL_OK;
}

/* Read signed 24-bit ADC result */
HAL_StatusTypeDef ADS1220_ReadRaw(int32_t *raw)
{
    uint8_t command = ADS1220_CMD_RDATA;
    uint8_t rx[3] = {0U};
    HAL_StatusTypeDef status;
    uint32_t value;

    if (raw == NULL){
        return HAL_ERROR;
    }

    HAL_GPIO_WritePin(ADS1220_CS_GPIO_Port,ADS1220_CS_Pin, GPIO_PIN_RESET);

    status = HAL_SPI_Transmit(&hspi1, &command, 1U, 100U);

    if (status == HAL_OK){
        status = HAL_SPI_Receive(&hspi1, rx, 3U, 100U);
    }

    HAL_GPIO_WritePin(ADS1220_CS_GPIO_Port,ADS1220_CS_Pin, GPIO_PIN_SET);

    if (status != HAL_OK){
        return status;
    }

    value = ((uint32_t)rx[0] << 16) | ((uint32_t)rx[1] << 8) | (uint32_t)rx[2];

    /* Sign-extend 24-bit result */
    if (value & 0x800000UL){
    	value |= 0xFF000000UL;
    }

    *raw = (int32_t)value;

    return HAL_OK;
}

/* Read ADC and calculate voltage */
HAL_StatusTypeDef ADS1220_ReadVoltage(int32_t *raw,int32_t *voltage_mV)
{
    HAL_StatusTypeDef status;

    if (raw == NULL || voltage_mV == NULL){
        return HAL_ERROR;
    }

    status = ADS1220_SendCommand(ADS1220_CMD_START);
    if (status != HAL_OK){
        return status;
    }

    status = ADS1220_WaitReady(100U);
    if (status != HAL_OK){
        return status;
    }

    status = ADS1220_ReadRaw(raw);
    if (status != HAL_OK){
        return status;
    }

    *voltage_mV = (int32_t)(((int64_t)(*raw) * ADS1220_VREF_MV) / ADS1220_FULL_SCALE );

    return HAL_OK;
}

