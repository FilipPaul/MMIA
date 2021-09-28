/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Auto-generated by STM32CubeIDE
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */
#include "stm32f0xx.h"
uint32_t morse = 0b10101001110111011100101010000000;

#if !defined(__SOFT_FP__) && defined(__ARM_FP)
  #warning "FPU is not initialized, but the project is compiling for an FPU. Please initialize the FPU before use."
#endif

void EXTI0_1_IRQHandler(void)
 {
	 if (EXTI->PR & EXTI_PR_PR0) { // check line 0 has triggered the IT
		 EXTI->PR |= EXTI_PR_PR0; // clear the pending bit
		 GPIOA->ODR ^= (1<<4); //toggle YELLOW LED
	 }
 }

int main(void)
{
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN | RCC_AHBENR_GPIOBEN | RCC_AHBENR_GPIOCEN; //enable clock
	GPIOA->MODER |= GPIO_MODER_MODER4_0; //PA4 (YELLOW LED)
	GPIOB->MODER |= GPIO_MODER_MODER0_0; //PB0 (BLUE LED)
	GPIOC->PUPDR |= GPIO_PUPDR_PUPDR0_0; //PC0, PULLUP (at the edge of the board)
	GPIOC->PUPDR |= GPIO_PUPDR_PUPDR0_0; //PC1, PULLUP

	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN; //enable system timer

	//External interupt settings for PC0 (button)
	SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI0_PC; // select PC0 for EXTI0
	EXTI->IMR |= EXTI_IMR_MR0; // mask
	EXTI->FTSR |= EXTI_FTSR_TR0; // trigger on falling edge
	NVIC_EnableIRQ(EXTI0_1_IRQn); // enable EXTI0_1

	GPIOB->BSRR = (1<<4); //toggle YELLOW LED

    while (1)
    {

    }
    
}
