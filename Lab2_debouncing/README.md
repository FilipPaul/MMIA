<h1>Lab 2: Debounce button</h1>
<h2>PART 1: Button click detection by interrupt </h2>

```c
#include "stm32f0xx.h"

#if !defined(__SOFT_FP__) && defined(__ARM_FP)
#warning "FPU is not initialized, but the project is compiling for an FPU. Please initialize the FPU before use."
#endif

void EXTI0_1_IRQHandler(void)
{
	if (EXTI->PR & EXTI_PR_PR0)
	{							 // check line 0 has triggered the IT
		EXTI->PR |= EXTI_PR_PR0; // clear the pending bit
		GPIOA->ODR ^= (1 << 4);	 //toggle YELLOW LED
	}
}

int main(void)
{
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN | RCC_AHBENR_GPIOBEN | RCC_AHBENR_GPIOCEN; //enable clock
	GPIOA->MODER |= GPIO_MODER_MODER4_0;										 //PA4 (YELLOW LED)
	GPIOB->MODER |= GPIO_MODER_MODER0_0;										 //PB0 (BLUE LED)
	GPIOC->PUPDR |= GPIO_PUPDR_PUPDR0_0;										 //PC0, PULLUP (at the edge of the board)
	GPIOC->PUPDR |= GPIO_PUPDR_PUPDR0_0;										 //PC1, PULLUP

	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN; //enable system timer

	//External interupt settings for PC0 (button)
	SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI0_PC; // select PC0 for EXTI0
	EXTI->IMR |= EXTI_IMR_MR0;					  // mask
	EXTI->FTSR |= EXTI_FTSR_TR0;				  // trigger on falling edge
	NVIC_EnableIRQ(EXTI0_1_IRQn);				  // enable EXTI0_1

	GPIOB->BSRR = (1 << 4); //toggle YELLOW LED

	while (1)
	{
	}
}
```
<img src="https://github.com/FilipPaul/MMIA/blob/main/Lab2_debouncing/debouncePart1.gif">