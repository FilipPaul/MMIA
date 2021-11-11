<h1>Lab 4: ADC</h1>
<h2>PART 1: display ADC value on 7seg display and make bargraf from LEDs</h2>
<p> přikládám pouze podstatné části z mainu, celý main je možno nalézt v src... Mám trochu jiné zapojení a 7seg displej zobrazuje
čísla od 0 do 330, to z důvodu, abych ověřil funčnost ADC převodníku s referenčním multimetrem</p><br>

main.c

```c

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "sct.h"

static volatile uint32_t raw_pot_value;

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_ADC_Init(void);

//CALBACK ISR for ADC
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc){
	 raw_pot_value = HAL_ADC_GetValue(hadc);
 }


//Main
int main(void)
{
  HAL_Init();
  SystemClock_Config();

  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_ADC_Init(); //INIT ADC

  sct_init(); //Init 7SEG driver

  //ADC SETTINGS
  HAL_ADCEx_Calibration_Start(&hadc); // autocalibration
  HAL_ADC_Start_IT(&hadc); //start ADC in interrupt mode (after each conversion starts again)

  while (1)
  {
	  //raw_pot_is 12 bit -> we want range from 0 to 330
	  sct_display_digit(raw_pot_value*330/( 1<<12 ) , raw_pot_value * 9 / (1<<12));

  }
}

```

sct.c

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

void sct_display_digit(uint32_t digit_to_display, uint8_t led_bargraf){
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
	reg |= (0b11111111) << (3*8); //my leds are connected in active low mode -> turn them off
	for (uint8_t i = 0; i < led_bargraf; i++)
	 {
		reg &= ~( 1 << (3*8+ i) ); //turn Number of leds from led_bargraf on
	 }
	reg |= (reg_values[digit_to_display / 100 % 10] << (2*8));
	reg |= (reg_values[digit_to_display / 10 % 10]  << (1*8));
	reg |= reg_values[digit_to_display / 1 % 10];
	sct_led(reg);
}

```


<img src="https://github.com/FilipPaul/MMIA/blob/main/Lab4_ADC/gifs/PART1.gif">




