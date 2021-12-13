/*
 * sct.c
 *
 *  Created on: Oct 22, 2021
 *      Author: Filip
 */

#include "stm32f0xx.h"
#include "sct.h"

//Macro to set/RESET PINS
#define sct_Data_IN(x) do { if (x) GPIOC->BSRR = (1 << 0); else GPIOC->BRR = (1 << 0); } while (0)
#define sct_latch(x) do { if (x) GPIOC->BSRR = (1 << 1); else GPIOC->BRR = (1 << 1); } while (0)
#define sct_CLK(x) do { if (x) GPIOC->BSRR = (1 << 2); else GPIOC->BRR = (1 << 2); } while (0)
#define sct_Output_Enable(x) do { if (x) GPIOC->BSRR = (1 << 3); else GPIOC->BRR = (1 << 3); } while (0)
#define delay_cycles(x) do { for (volatile uint32_t i = 0; i < x; i++){} } while (0)
#define DELAY_CYCLE 1000

void sct_led(uint32_t to_display){
    for (uint8_t i = 0; 32 > i ; i++)
    {
       sct_Data_IN(to_display & 1); //insert LSB into this function
       to_display = to_display >> 1;
       delay_cycles(DELAY_CYCLE);
       //CLK pulse
       sct_CLK(1);
       delay_cycles(DELAY_CYCLE);
       sct_CLK(0);
    }
    //Latch pulse
		 delay_cycles(DELAY_CYCLE);
		 sct_latch(1);
		 delay_cycles(DELAY_CYCLE);
		 sct_latch(0);

}

void sct_init(void){
    RCC->AHBENR |= RCC_AHBENR_GPIOCEN; //enable clock
    GPIOC->MODER |= GPIO_MODER_MODER3_0 |GPIO_MODER_MODER2_0 | GPIO_MODER_MODER1_0 | GPIO_MODER_MODER0_0 ; //PC0-PC3 pin output
	sct_Output_Enable(0);
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

