<h1>Lab 3: 7-SEG display</h1>
<h2>PART 1 and 2: Driver initialization and counting</h2>
<p> Vzhledem k tomu, že nemám přídavnou desku tak a zapojení je poskládané z věcí, které se mi válí doma, tak jsem použil 4x shiftregister SN74HC595N místo SCT2024CSTG. Dále protože to mám zapojené trochu jinak, proto se liší lookup table pro jednotlivé digity. Z důvodu zapojení na Breadboardu je do funkce 7seg Driveru přidáno i delay_cycles(DELAY_CYCLE); ...</p><br>

<h3>sct.c</h3>

```c
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
    //Look Up table for 7seg DIGITS -> At least for me this is more readable than the lookup from assignment
    // First 8 LEDs are ignored..
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


```
<br>
<h3>main.c</h3>


```c
#include "sct.h"
#include "stm32f0xx.h"
#include "stdint.h"

#if !defined(__SOFT_FP__) && defined(__ARM_FP)
#warning "FPU is not initialized, but the project is compiling for an FPU. Please initialize the FPU before use."
#endif

int main(void)
{
	sct_init();
	sct_led(0b10101010111000001100010010110000); // bYE
	for (volatile uint32_t j = 0; j < 1000000; j++)
	{
	}
	while (1)
	{
		for (uint16_t i = 0; i < 1000; i += 111)
		{
			sct_display_digit(i);
			for (volatile uint32_t j = 0; j < 300000; j++)vg
			{
			}
		}
	}
}


```
<img src="https://github.com/FilipPaul/MMIA/blob/main/Lab3_DISPLAY_7SEG/gifs/PART_1_AND_2.gif">

<br>
<h2>PART 3: 7SEG driver with the usage of HAL libraries</h2>
<h3>sct.c</h3>


```c
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
```
<br>
<h3>main.c</h3>
<p> Kopírovat celý main, je podle mě zbytečně nepřehledné, proto přikládám jen část..<p>


```c

  /* Private includes ----------------------------------------------------------*/
  /* USER CODE BEGIN Includes */
  #include "sct.h"
  /* USER CODE END Includes */
                .
                .
                .
                .
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  sct_init();
  sct_led(0b10101010111000001100010010110000);
  HAL_Delay(1000);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
		for (uint16_t i = 0; i < 1000; i += 111)
		{
			sct_display_digit(i);
			HAL_Delay(500);
		}
    /* USER CODE BEGIN 3 */
  }

```
<img src="https://github.com/FilipPaul/MMIA/blob/main/Lab3_DISPLAY_7SEG/gifs/PART_3.gif">


