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
    //tady jsem  udělal chybičku, které je opravená v další části.... na
    //GPIOC->PUPDR |= GPIO_PUPDR_PUPDR1_0; //PC1, PULLUP
    										     
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

<h2>PART 2: Button debouncing by falling edge detection with folowing idle time interval </h2>

```c
#include "stm32f0xx.h"

#if !defined(__SOFT_FP__) && defined(__ARM_FP)
#warning "FPU is not initialized, but the project is compiling for an FPU. Please initialize the FPU before use."
#endif

#define LED_TIME_BLINK 500
#define BUTTON_DEBOUNCE_TIME 40
#define LED_TIME_BLINK_SHORT 100
#define LED_TIME_BLINK_LONG 1000

void EXTI0_1_IRQHandler(void)
{
	if (EXTI->PR & EXTI_PR_PR0)
	{							 // check line 0 has triggered the IT
		EXTI->PR |= EXTI_PR_PR0; // clear the pending bit
		GPIOA->ODR ^= (1 << 4);	 //toggle YELLOW LED
	}
}

//i know these variables should be local in functions, but i need them to reset with Tick together,
//with this approach, i think this code improves readability..
static volatile uint32_t delay = 0;
static volatile uint32_t delay_debounce_button = 0;
static volatile uint32_t delay_debounce_button_long = 0;
static volatile uint32_t Tick = 0;
static volatile uint32_t off_time;
static volatile uint32_t  off_time_long;

void SysTick_Handler(void){
	Tick++;
	if (Tick == 0x0fffffff)//reset Tick an to prevent freezing also  reset delay variable
	{
		Tick = 0;
		delay = 0;
		delay_debounce_button = 0;
		delay_debounce_button_long = 0;
		off_time = LED_TIME_BLINK_SHORT;
		off_time_long = LED_TIME_BLINK_LONG;
	}
	
}

void blink(void){ //function blikac() from

	if (Tick > delay + LED_TIME_BLINK)
	{
		GPIOA->ODR ^= (1<<4); //toggle YELLOW LED
		delay = Tick;

		
	}
	
}

void debounce(void){ //function blikac() from

	if (Tick > delay_debounce_button + BUTTON_DEBOUNCE_TIME)
	{
		static uint32_t in_progres_flag = 0;
		static uint32_t pre_state;
		uint32_t new_state = GPIOC->IDR & (1<<0); //read button PC0
		if ((pre_state == 1)&& (new_state ==0)){ //falling edge
			off_time = Tick + LED_TIME_BLINK_SHORT;
			GPIOB->BSRR = ( 1<< 0); //set BLUE LED ON
			in_progres_flag = 1;
		}
		if((Tick > off_time) && (in_progres_flag == 1))//flag prevents to turn LED of from another function
		{
			GPIOB->BRR = (1<<0); //TURN LED OFF
			in_progres_flag = 0;
		}
		pre_state = new_state;
		delay_debounce_button = Tick;
	}
}

void debounceLong(void){ //function blikac() from

	if (Tick > delay_debounce_button_long + BUTTON_DEBOUNCE_TIME)
	{
		static uint32_t in_progres_flag = 0;
		static uint32_t pre_state_long;
		//(GPIOC->IDR & (1<<1) returns 0b00....0010 -> therefore the shift right (>> 1) to get simply 0 or 1
		uint32_t new_state_long = ((GPIOC->IDR & (1<<1)) >> 1); //read button PC1
		if ((pre_state_long == 1)&& (new_state_long ==0)){ //falling edge
			off_time_long = Tick + LED_TIME_BLINK_LONG;
			GPIOB->BSRR = ( 1<< 0); //set BLUE LED ON
			in_progres_flag = 1;
		}
		if ((Tick > off_time_long) && (in_progres_flag == 1))//flag prevents to turn LED of from another function
		{
			GPIOB->BRR = (1<<0); //TURN LED OFF
			in_progres_flag = 0;
		}
		pre_state_long = new_state_long;
		delay_debounce_button_long = Tick;

	}
}



int main(void)
{
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN | RCC_AHBENR_GPIOBEN | RCC_AHBENR_GPIOCEN; //enable clock
	GPIOA->MODER |= GPIO_MODER_MODER4_0;										 //PA4 (YELLOW LED)
	GPIOB->MODER |= GPIO_MODER_MODER0_0;										 //PB0 (BLUE LED)
	GPIOC->PUPDR |= GPIO_PUPDR_PUPDR0_0;										 //PC0, PULLUP (at the edge of the board)
	GPIOC->PUPDR |= GPIO_PUPDR_PUPDR1_0;										 //PC1, PULLUP

	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN; //enable system timer

	SysTick_Config(8000); //8MHz -> each 8000 ticks -> every 1ms
/*
	//External interrupt settings for PC0 (button)
	SYSCFG->EXTICR[0] |= SYSCFG_EXTICR1_EXTI0_PC; // select PC0 for EXTI0
	EXTI->IMR |= EXTI_IMR_MR0;					  // mask
	EXTI->FTSR |= EXTI_FTSR_TR0;				  // trigger on falling edge
	NVIC_EnableIRQ(EXTI0_1_IRQn);				  // enable EXTI0_1
*/
	while (1)
	{
		blink();
		debounce();
		debounceLong();
	}
}
```

<img src="https://github.com/FilipPaul/MMIA/blob/main/Lab2_debouncing/debouncePart2.gif" rotate="90">