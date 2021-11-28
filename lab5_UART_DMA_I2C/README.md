<h1>Lab 5: UART, DMA, I2C EEPROM</h1>
<h2>PART 1: Commands from UART</h2>
main.c

```c
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h> //for printf
#include <string.h> //strtoc, strcmp



/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart2;
DMA_HandleTypeDef hdma_usart2_rx;

/* USER CODE BEGIN PV */
#define RX_BUFFER_LEN 64
#define CMD_BUFFER_LEN 64
static uint8_t uart_rx_buffer[RX_BUFFER_LEN];
static volatile uint16_t uart_rx_pointer_read = 0;
#define uart_rx_pointer_write (RX_BUFFER_LEN - hdma_usart2_rx.Instance->CNDTR)

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */
static void uart_process_command(char *cmd)
{
	//printf is overloaded to print data into UART2
	//printf("received: '%s'\n", cmd);

	char *command;
	command = strtok(cmd, " "); //similarly to python's list.split, but takes only first one..
	if (  strcasecmp(command, "HELLO") == 0   ){
		printf("I'm ready!\n");
	}

	else if (  strcasecmp(command, "LED2") == 0   )
	{
		command = strtok(NULL, " "); //take last result from strtok -> 2nd word
		printf("I'm ready!\n");
		if (  strcasecmp(command, "ON") == 0   ){
			HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin,GPIO_PIN_SET );
			printf("Turning LED2 ON\n");
		}
		else if (  strcasecmp(command, "OFF") == 0   ){
			HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin,GPIO_PIN_RESET );
			printf("Turning LED2 OFF\n");
		}
	}

	else if (  strcasecmp(command, "LED1") == 0   )
	{
		command = strtok(NULL, " "); //take last result from strtok -> 2nd word
		printf("I'm ready!\n");
		if (  strcasecmp(command, "ON") == 0   ){
			HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin,GPIO_PIN_SET );
			printf("Turning LED1 ON\n");
		}
		if (  strcasecmp(command, "OFF") == 0   ){
			HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin,GPIO_PIN_RESET );
			printf("Turning LED1 OFF\n");
		}
	}

	else if (  strcasecmp(command, "STATUS") == 0   )
	{
		if (HAL_GPIO_ReadPin(LD1_GPIO_Port,LD1_Pin ) == 0 )
			printf("status LED 1: OFF");
		else {
			printf("status LED 1: ON");
		}

		if (HAL_GPIO_ReadPin(LD2_GPIO_Port,LD2_Pin ) == 0 )
			printf("status LED 2: OFF\n");
		else {
			printf("status LED 2: ON\n");
		}

	}

	else{
		printf("Unknown command\n");
	}

}

int _write(int file, char const *buf, int n)
{
	HAL_UART_Transmit(&huart2, (uint8_t*)(buf), n, HAL_MAX_DELAY);
	return n; //size
}

static void uart_byte_avaiable(uint8_t c)
{
	 static uint16_t cnt;
	 static char data[CMD_BUFFER_LEN];
	 // 32 - 126 represents common ASCI characters

	 if (cnt < CMD_BUFFER_LEN && c >= 32 && c <= 126){

		 data[cnt] = c; //put received character into data array
		 cnt++; //increment pointer of array
	 }
	 if ((c == '\n' || c == '\r' || c == '*') && cnt > 0) {
		 data[cnt] = '\0'; //create C string from received character
		 uart_process_command(data);
		 cnt = 0;
	 }
}

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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
  MX_DMA_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  HAL_UART_Receive_DMA(&huart2, uart_rx_buffer, RX_BUFFER_LEN);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

	while(uart_rx_pointer_read != uart_rx_pointer_write){
		  uint8_t b = uart_rx_buffer[uart_rx_pointer_read];
		  uart_rx_pointer_read++;

		  if (uart_rx_pointer_read >= RX_BUFFER_LEN){
			  uart_rx_pointer_read = 0;
		  }
		  uart_byte_avaiable(b);

	  }
/*

	  //LOOPBACK
	  uint8_t UARTchar;
	  //take received char into UARTchar
	  HAL_UART_Receive(&huart2, &UARTchar, 1, HAL_MAX_DELAY);

	  //Transmitt UARTchar
	  HAL_UART_Transmit(&huart2, &UARTchar, 1, HAL_MAX_DELAY);

*/
  }
  /* USER CODE END 3 */
}

```

<img src="https://github.com/FilipPaul/MMIA/blob/main/Lab_4_ADC/gifs/PART2.gif">
