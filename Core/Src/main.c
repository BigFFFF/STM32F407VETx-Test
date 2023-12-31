/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "filter.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define ADC_DATA_MAX	10
#define KEY_MAX 		3
#define PWM_PULSE_MAX 	500
#define PWM_PULSE_MIN 	0
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
KEY_STATE_T Key[KEY_MAX] = {{.keyState = KEY_CHECK, .keyFlag = false, .keyPin = KEY_1_Pin},
					  	  	{.keyState = KEY_CHECK, .keyFlag = false, .keyPin = KEY_2_Pin},
							{.keyState = KEY_CHECK, .keyFlag = false, .keyPin = KEY_3_Pin}};

uint16_t led_1_pwm_pulse = 0;
int ADC_SINGLE[CH_MAX] = {0};
int ADC_DATA[CH_MAX][ADC_DATA_MAX] = {0};
int ADC_Value[CH_MAX] = {0};
int ADC_Filter[CH_MAX] = {0};
int LED_NUM[CH_MAX] = {0};

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
static void led_1_pwm(void);
static void led_2_pwm(void);
static void key_scan(void);
static void key_get(void);
static void adc_scan(void);
static void adc_filter(int ch_num, uint32_t avg);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
//对printf的重定向
#ifdef __GNUC__
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif
PUTCHAR_PROTOTYPE
{
	RS485_TX_EN;
	HAL_UART_Transmit(&huart2 , (uint8_t *)&ch, 1, 0xFFFF);
	RS485_RX_EN;

	return ch;
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  __disable_irq();
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
  MX_TIM1_Init();
  MX_TIM10_Init();
  MX_TIM11_Init();
  MX_ADC1_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_4);
  HAL_TIM_Base_Start_IT(&htim10);
  HAL_TIM_Base_Start_IT(&htim11);
  __enable_irq();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	key_get();
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
//LED_1渐变,呼吸灯，使用TIM11调频，T=2s
static void led_1_pwm() {
	static bool led_1_state = false;
	if (led_1_state == false) {
		led_1_pwm_pulse++;
		led_1_state = (led_1_pwm_pulse < PWM_PULSE_MAX ? false : true);
	}
	else {
		led_1_pwm_pulse--;
		led_1_state = (led_1_pwm_pulse != PWM_PULSE_MIN ? true : false);
	}
	TIM1->CCR3 = led_1_pwm_pulse;
}

//LED_2改变亮度�?????????3挡可调）
static void led_2_pwm() {
	static uint16_t pwm_pulse;
	pwm_pulse += (pwm_pulse != PWM_PULSE_MAX ? (PWM_PULSE_MAX * 0.25) : (-PWM_PULSE_MAX));
	TIM1->CCR4 = pwm_pulse;
}

//按键扫描+消抖
static void key_scan() {
	for (int i = 0; i < 3; i++) {
		switch(Key[i].keyState){
		case KEY_CHECK:
			if (HAL_GPIO_ReadPin(KEY_GPIO_Port, Key[i].keyPin) ==  GPIO_PIN_RESET) {
				Key[i].keyState = KEY_COMFIRM;
			}
			break;
		case KEY_COMFIRM:
			if (HAL_GPIO_ReadPin(KEY_GPIO_Port, Key[i].keyPin) ==  GPIO_PIN_RESET) {
				Key[i].keyFlag = true;
				Key[i].keyState = KEY_RELEASE;
			}
			else {
				Key[i].keyState = KEY_CHECK;
			}
			break;
		case KEY_RELEASE:
			if (HAL_GPIO_ReadPin(KEY_GPIO_Port, Key[i].keyPin) ==  GPIO_PIN_SET) {
				Key[i].keyState = KEY_CHECK;
			}
			break;
		default:
			break;
		}
	}
}

//执行按键操作
static void key_get() {
	//key_1触发|LED_1�?????????�?????????
	if (Key[0].keyFlag == true) {
		Key[0].keyFlag = false;
		static bool led_1_state;
		if (!led_1_state){
			led_1_state = (HAL_TIM_PWM_Start(&htim1,TIM_CHANNEL_3) == HAL_OK ? true : false);
		}
		else {
			led_1_state = (HAL_TIM_PWM_Stop(&htim1,TIM_CHANNEL_3) == HAL_OK ? false : true);
		}
	}
	//key_2触发|LED_2�?????????�?????????
	else if (Key[1].keyFlag == true) {
		Key[1].keyFlag = false;
		led_2_pwm();
	}
	//key_3触发|LED_3�?????????�?????????
	else if (Key[2].keyFlag == true) {
		Key[2].keyFlag = false;
		HAL_GPIO_TogglePin(LED_3_GPIO_Port, LED_3_Pin);
	}
}

//定时器终端回�?????
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	//100Hz
	if (htim->Instance == TIM10) {
		key_scan();
		//更改定时器时，需要更改adc_scan定时器开关号和filter计数�?
		adc_scan();
	}
	//500Hz
	if (htim->Instance == TIM11) {
		led_1_pwm();
	}
}

//ADC多�?�道轮询及转�?
static void adc_scan() {
	static uint16_t data_num[CH_MAX];
	static uint16_t data_sum[CH_MAX];
	static uint16_t data_max[CH_MAX] = {V_MIN};
	static uint16_t data_min[CH_MAX] = {V_MAX};

	HAL_TIM_Base_Stop_IT(&htim10);

	//轮询通道获取ADC�?
	for (int ch_num = 0; ch_num < CH_MAX; ch_num++) {
		HAL_ADC_Start(&hadc1);
		HAL_ADC_PollForConversion(&hadc1,50);
		ADC_SINGLE[ch_num] = HAL_ADC_GetValue(&hadc1);
		data_max[ch_num] = (data_max[ch_num] > ADC_SINGLE[ch_num] ? data_max[ch_num] : ADC_SINGLE[ch_num]);
		data_min[ch_num] = (data_min[ch_num] < ADC_SINGLE[ch_num] ? data_min[ch_num] : ADC_SINGLE[ch_num]);
		ADC_DATA[ch_num][data_num[ch_num]] = ADC_SINGLE[ch_num];
		data_sum[ch_num] += ADC_DATA[ch_num][data_num[ch_num]];
		data_num[ch_num]++;
		//当数组满时，去极值求均�?�滤�?
		if (data_num[ch_num] == ADC_DATA_MAX) {
			data_sum[ch_num] -= (data_max[ch_num] + data_min[ch_num]);
			//右移三位求均�?
			adc_filter(ch_num, data_sum[ch_num] >> 3);
			data_sum[ch_num] = 0;
			data_max[ch_num] = V_MIN;
			data_min[ch_num] = V_MAX;
			data_num[ch_num] = 0;
		}
	}
	HAL_TIM_Base_Start_IT(&htim10);
}

//ADCtoLED过滤
static void adc_filter(int ch_num, uint32_t avg) {
    ADC_Value[ch_num] = avg;
    ch[ch_num].adc_x = ADC_Value[ch_num];
    ADC_Filter[ch_num] = filter_x(&ch[ch_num], V_SINGE);
    led[ch_num].adc_x = ADC_Filter[ch_num];
    LED_NUM[ch_num] = filter_led(&led[ch_num], V_SINGE);
    //printf("CH:%d, ADC_Value:%d, ADC_Filter_Value:%d, LED_NUM:%d\r\n", ch_num, ADC_Value[ch_num], ADC_Filter[ch_num], LED_NUM[ch_num]);
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
