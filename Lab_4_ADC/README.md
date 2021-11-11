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


<img src="https://github.com/FilipPaul/MMIA/blob/main/Lab_4_ADC/gifs/PART1.gif">




<h2>PART 2 3 and 4: Filter ADC, Temperature, Voltage</h2>
main.c

```c
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "sct.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define AVG_Q 12
#define TEMP110_CAL_ADDR ((uint16_t*) ((uint32_t) 0x1FFFF7C2))
#define TEMP30_CAL_ADDR ((uint16_t*) ((uint32_t) 0x1FFFF7B8))
/* Internal voltage reference calibration value address */
#define VREFINT_CAL_ADDR ((uint16_t*) ((uint32_t) 0x1FFFF7BA))
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */
static volatile uint32_t raw_pot_value;
static volatile uint32_t raw_temp;
static volatile uint32_t raw_volt;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_ADC_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
 void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc){
	 static uint8_t channel = 0;
	 if (channel == 0){ //get pot value
		 static uint32_t avg_pot;
		 raw_pot_value = avg_pot >> AVG_Q; //Avg_pot is val
		 avg_pot -= raw_pot_value;
		 avg_pot += HAL_ADC_GetValue(hadc);
	 }

	 else if (channel == 1){ //TEMPERATURE
		 raw_temp = HAL_ADC_GetValue(hadc);
	 }

	 else if (channel == 2){ //Voltage
		 raw_volt = HAL_ADC_GetValue(hadc);
	 }

	 if(__HAL_ADC_GET_FLAG(hadc,ADC_FLAG_EOS)) {
		 channel = 0;
	 }

	 else {
		 channel++;
	 }
 }
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
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
  MX_ADC_Init();
  /* USER CODE BEGIN 2 */
  sct_init(); //Init 7SEG driver
  HAL_ADCEx_Calibration_Start(&hadc); // autocalibration
  HAL_ADC_Start_IT(&hadc); //start ADC in interrupt mode (after each conversion starts again)
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

	  static enum {SHOW_POT, SHOW_VOLT, SHOW_TEMP} state = SHOW_POT;
	  static uint32_t delay;

	  if (state == SHOW_POT){
		  //raw_pot_is 12 bit -> we want range from 0 to 900
		  sct_display_digit(raw_pot_value*999/( 1<<12 ) , raw_pot_value * 9 / (1<<12));
	  }
	  else if (state == SHOW_TEMP ){
		  int32_t temperature = (raw_temp - (int32_t)(*TEMP30_CAL_ADDR));
		  temperature = temperature * (int32_t)(110 - 30);
		  temperature = temperature / (int32_t)(*TEMP110_CAL_ADDR - *TEMP30_CAL_ADDR);
		  temperature = temperature + 30;

		  sct_display_digit(temperature , temperature * 9/60);
	  }

	  else if (state == SHOW_VOLT ){
		  uint32_t voltage = 330 * (*VREFINT_CAL_ADDR) / raw_volt;
		  sct_display_digit(voltage , voltage * 9/3);
	  }

	  if (HAL_GetTick() > delay + 2000) state = SHOW_POT;

	  if (!HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_10)) {
		  state = SHOW_TEMP;
		  delay = HAL_GetTick();
	  }
	  if (!HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13)) {
		  state = SHOW_VOLT;
		  delay = HAL_GetTick();
	  }
  }
  /* USER CODE END 3 */
}

```

<img src="https://github.com/FilipPaul/MMIA/blob/main/Lab_4_ADC/gifs/PART2.gif">
