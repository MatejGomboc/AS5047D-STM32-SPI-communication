/*
 * AS4047D.c
 *
 *  Created on: 24. avg. 2016
 *      Author: user
 */

#include "AS4047D.h"
#include <stdint.h>
#include "spi.h"
#include "gpio.h"


uint16_t parity(uint16_t x)
{
	uint16_t parity = 0;

	while(x != 0)
	{
		parity ^= x;
		x >>= 1;
	}

	return (parity & 0x1);
}

void AS5047D_Write(GPIO_TypeDef* CS_GPIO_Port, uint16_t CS_GPIO_Pin, uint16_t address, uint16_t data)
{
	if (parity(address & 0x3FFF) == 1) address = address | 0x8000; // set parity bit
	//address = address & (WR | 0x8000);  // its  a write command and don't change the parity bit (0x8000)

	HAL_GPIO_WritePin(CS_GPIO_Port, CS_GPIO_Pin, GPIO_PIN_RESET);

	HAL_Delay(1);

	if (HAL_SPI_Transmit(&hspi2, (uint8_t*) &address, 2, 100) != HAL_OK)
	{
		Error_Handler();
	}

	HAL_GPIO_WritePin(CS_GPIO_Port, CS_GPIO_Pin, GPIO_PIN_SET);

	HAL_Delay(1);

	if (parity(data & 0x3FFF) == 1) data = data | 0x8000; // set parity bit
	//data = data & (WR | 0x8000); // its a write command and don't change the parity bit (0x8000)

	HAL_GPIO_WritePin(CS_GPIO_Port, CS_GPIO_Pin, GPIO_PIN_RESET);

	HAL_Delay(1);

	if (HAL_SPI_Transmit(&hspi2, (uint8_t*) &data, 2, 100) != HAL_OK)
	{
		Error_Handler();
	}

	HAL_GPIO_WritePin(CS_GPIO_Port, CS_GPIO_Pin, GPIO_PIN_SET);
}

uint16_t AS5047D_Read(GPIO_TypeDef* CS_GPIO_Port, uint16_t CS_GPIO_Pin, uint16_t address)
{
	if (parity(address | AS4047D_RD) == 1) address = address | 0x8000; // set parity bit
	address = address | AS4047D_RD; // it's a read command

	HAL_GPIO_WritePin(CS_GPIO_Port, CS_GPIO_Pin, GPIO_PIN_RESET);

	HAL_Delay(1);

	if (HAL_SPI_Transmit(&hspi2, (uint8_t*) &address, 1, 100) != HAL_OK)
	{
		Error_Handler();
	}

	HAL_GPIO_WritePin(CS_GPIO_Port, CS_GPIO_Pin, GPIO_PIN_SET);

	HAL_Delay(1);

	HAL_GPIO_WritePin(CS_GPIO_Port, CS_GPIO_Pin, GPIO_PIN_RESET);

	HAL_Delay(1);

	uint16_t data = 0;

	if (HAL_SPI_Receive(&hspi2, (uint8_t*) &data, 1, 100) != HAL_OK)
	{
		Error_Handler();
	}

	HAL_GPIO_WritePin(CS_GPIO_Port, CS_GPIO_Pin, GPIO_PIN_SET);

	data = data & 0x3FFF;  // filter bits outside data, strip bit 14..15
	return data;
}

void AS5047D_Check_Transmission_Error(void)
{
	/** Check if transmission error **/
	if(AS5047D_Read(AS4047D_CS1_Port, AS4047D_CS1_Pin, AS4047D_ERRFL) != 0)
	{
		Error_Handler();
	}
}

void AS5047D_SetZero(void)
{
	/** Check diagnostics reg **/
	uint16_t DIAAGC = AS5047D_Read(AS4047D_CS1_Port, AS4047D_CS1_Pin, AS4047D_DIAAGC);
	//AS5047D_Check_Transmission_Error();
	//if((AS5047D_Check_MAG_TooLow(DIAAGC)) || (AS5047D_Check_MAG_TooHigh(DIAAGC)) || (AS5047D_Check_CORDIC_Overflow(DIAAGC)) || !(AS5047D_Check_LF_finished(DIAAGC)))
	//{
		//Error_Handler();
	//}

	/** Get uncompensated angle reg value **/
	uint16_t ANGLEUNC = AS5047D_Read(AS4047D_CS1_Port, AS4047D_CS1_Pin, AS4047D_ANGLEUNC);
	//AS5047D_Check_Transmission_Error();

	/** Write to zero pos regs **/
	AS5047D_Write(AS4047D_CS1_Port, AS4047D_CS1_Pin , AS4047D_ZPOSM, (ANGLEUNC >> 6) & 0x00FF);
	//AS5047D_Check_Transmission_Error();
	AS5047D_Write(AS4047D_CS1_Port, AS4047D_CS1_Pin , AS4047D_ZPOSL, ANGLEUNC & 0x003F);
	//AS5047D_Check_Transmission_Error();
}

uint16_t AS5047D_GetZero(void)
{
	uint16_t ZPOSM = 0;
	uint16_t ZPOSL = 0;

	ZPOSM = AS5047D_Read(AS4047D_CS1_Port, AS4047D_CS1_Pin, AS4047D_ZPOSM);
	//AS5047D_Check_Transmission_Error();
	ZPOSL = AS5047D_Read(AS4047D_CS1_Port, AS4047D_CS1_Pin, AS4047D_ZPOSL);
	//AS5047D_Check_Transmission_Error();

	return (((ZPOSM << 6) & 0x3FC0) | (ZPOSL & 0x003F));
}

uint8_t AS5047D_Get_AGC_Value(void)
{
	/** Read diagnostics reg **/
	uint16_t DIAAGC = AS5047D_Read(AS4047D_CS1_Port, AS4047D_CS1_Pin, AS4047D_DIAAGC);
	//AS5047D_Check_Transmission_Error();
	return (uint8_t)((DIAAGC >> 8) & 0x00FF);
}

void AS5047D_Init(void)
{
	/* Initiaize AS4047D */
	AS5047D_Write(AS4047D_CS1_Port, AS4047D_CS1_Pin , AS4047D_SETTINGS1, 0b00000101);
	//AS5047D_Check_Transmission_Error();
	AS5047D_Write(AS4047D_CS1_Port, AS4047D_CS1_Pin , AS4047D_SETTINGS2, 0b00000000);
	//AS5047D_Check_Transmission_Error();
}

uint16_t AS5047D_Get_CORDICMAG_Value(void)
{
	uint16_t CORDICMAG = AS5047D_Read(AS4047D_CS1_Port, AS4047D_CS1_Pin, AS4047D_CORDICMAG);
	//AS5047D_Check_Transmission_Error();
	return CORDICMAG;
}

uint16_t AS5047D_Get_ANGLEUNC_Value(void)
{
	uint16_t ANGLEUNC = AS5047D_Read(AS4047D_CS1_Port, AS4047D_CS1_Pin, AS4047D_ANGLEUNC);
	//AS5047D_Check_Transmission_Error();
	return ANGLEUNC;
}

uint16_t AS5047D_Get_ANGLECOM_Value(void)
{
	uint16_t ANGLECOM = AS5047D_Read(AS4047D_CS1_Port, AS4047D_CS1_Pin, AS4047D_ANGLECOM);
	//AS5047D_Check_Transmission_Error();
	return ANGLECOM;
}

float AS5047D_Get_True_Angle_Value(void)
{
	return((float)AS5047D_Get_ANGLEUNC_Value() * 360.0f / 16383.0f);
	//return((float)AS5047D_Get_ANGLECOM_Value() * 360.0f / 16383.0f);
}
