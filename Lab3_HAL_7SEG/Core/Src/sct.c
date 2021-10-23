/*
 * sct.c
 *
 *  Created on: Oct 22, 2021
 *      Author: Filip
 */

#include "stm32f0xx.h"
#include "sct.h"
#include "main.h"


//Macro to set/RESET PINS
#define delay_cycles(x) do { for (volatile uint32_t i = 0; i < x; i++){} } while (0)
#define DELAY_MS 1

void sct_led(uint32_t to_display){
    for (uint8_t i = 0; 32 > i ; i++)
    {
       HAL_GPIO_WritePin(SCT_DATA_IN_GPIO_Port, SCT_DATA_IN_Pin, to_display & 1);
       to_display = to_display >> 1;
       HAL_Delay(DELAY_MS);
       //CLK pulse
       HAL_GPIO_WritePin(SCT_CLK_GPIO_Port, SCT_CLK_Pin,1);
       delay_cycles(DELAY_MS);
       HAL_GPIO_WritePin(SCT_CLK_GPIO_Port, SCT_CLK_Pin,0);
    }
    //Latch pulse
    	 delay_cycles(DELAY_MS);
		 HAL_GPIO_WritePin(SCT_LATCH_GPIO_Port, SCT_LATCH_Pin,1);
		 delay_cycles(DELAY_MS);
		 HAL_GPIO_WritePin(SCT_LATCH_GPIO_Port, SCT_LATCH_Pin,0);

}

void sct_init(void){
	HAL_GPIO_WritePin(SCT_OUTPUT_ENABLED_GPIO_Port, SCT_OUTPUT_ENABLED_Pin,0);
    sct_led(0);
}

void sct_display_digit(uint32_t digit_to_display){
	static const uint32_t reg_values[10] = {
		0b10000001, //0
		0b11001111, //1
		0b10010010, //2
		0b10000110, //3
		0b11001100, //4
		0b10100100, //5
		0b10100000, //6
		0b10001111, //7
		0b10000000, //8
		0b10000100, //9
	 };

	uint32_t reg = 0;
	reg |= (reg_values[digit_to_display / 100 % 10] << 2*8);
	reg |= (reg_values[digit_to_display / 10 % 10]  << 1*8);
	reg |= reg_values[digit_to_display / 1 % 10];
	sct_led(reg);
}

