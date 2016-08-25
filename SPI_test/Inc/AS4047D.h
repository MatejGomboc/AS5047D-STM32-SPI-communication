/*
 * AS4047D.h
 *
 *  Created on: 24. avg. 2016
 *      Author: user
 */

#ifndef AS4047D_H_
#define AS4047D_H_

#include <stdint.h>
#include "gpio.h"

// AS4047D Register Addresses

/** volatile **/
#define AS4047D_NOP 0x0000
#define AS4047D_ERRFL 0x0001
#define AS4047D_PROG 0x0003
#define AS4047D_DIAAGC 0x3FFC
#define AS4047D_CORDICMAG 0x3FFD
#define AS4047D_ANGLEUNC 0x3FFE
#define AS4047D_ANGLECOM 0x3FFF

/** non-volatile **/
#define AS4047D_ZPOSM 0x0016
#define AS4047D_ZPOSL 0x0017
#define AS4047D_SETTINGS1 0x0018
#define AS4047D_SETTINGS2 0x0019

#define AS4047D_RD 0x4000    // bit 14 = "1" is Read + parity even
#define AS4047D_WR 0x3FFF    // bit 14 = "0" is Write

#define AS4047D_CS1_Port GPIOB
#define AS4047D_CS1_Pin GPIO_PIN_12

#define AS4047D_CS2_Port GPIOC
#define AS4047D_CS2_Pin GPIO_PIN_6

extern void AS5047D_Init(void);

extern void AS5047D_Write(GPIO_TypeDef* CS_GPIO_Port, uint16_t CS_GPIO_Pin, uint16_t address, uint16_t data);
extern uint16_t AS5047D_Read(GPIO_TypeDef* CS_GPIO_Port, uint16_t CS_GPIO_Pin, uint16_t address);

extern void AS5047D_Check_Transmission_Error(void);
extern void AS5047D_SetZero(void);
extern uint16_t AS5047D_GetZero(void);
extern uint8_t AS5047D_Get_AGC_Value(void);

extern uint16_t AS5047D_Get_CORDICMAG_Value(void);
extern uint16_t AS5047D_Get_ANGLEUNC_Value(void);
extern uint16_t AS5047D_Get_ANGLECOM_Value(void);

extern float AS5047D_Get_True_Angle_Value(void);

#define AS5047D_Check_MAG_TooLow(DIAAGC)      ((DIAAGC >> 11) & 0x0001)
#define AS5047D_Check_MAG_TooHigh(DIAAGC)     ((DIAAGC >> 10) & 0x0001)
#define AS5047D_Check_CORDIC_Overflow(DIAAGC) ((DIAAGC >> 9)  & 0x0001)
#define AS5047D_Check_LF_finished(DIAAGC)     ((DIAAGC >> 8)  & 0x0001)

#endif /* AS4047D_H_ */
