/*
 * @version: V1.0.0
 * @Author: harmsworth
 * @Date: 2023-03-27 13:07:17
 * @LastEditors: harmsworth
 * @LastEditTime: 2023-03-27 14:49:57
 * @company: null
 * @Mailbox: jojoharmsworth@gmail.com
 * @FilePath: \MDK-ARMg:\Program\Stm32\Projects\OLED_Menu_u8g2\Core\Src\main.c
 * @Descripttion:
 */
/*
 * @version: V1.0.0
 * @Author: harmsworth
 * @Date: 2023-03-20 09:51:44
 * @LastEditors: harmsworth
 * @LastEditTime: 2023-03-27 13:44:37
 * @company: null
 * @Mailbox: jojoharmsworth@gmail.com
 * @FilePath: \MDK-ARMg:\Program\Stm32\Projects\OLED_Menu_u8g2\Core\Src\main.c
 * @Descripttion:
 */
/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
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
#include "dma.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "oled.h"
#include "u8g2.h"
#include "u8x8.h"
#include "key.h"
#include "ui.h"
#include "directory.h"
#include "bh1750.h"
#include "dht11.h"
#include <stdio.h>
#include <string.h>
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

/* USER CODE BEGIN PV */
uint8_t temp = 22;
uint8_t humi = 22;
u8g2_t u8g2;

struct
{
  menu_t *current_pointer; // 当前链表位置
  int ctrl;
  char input;
} sys_info;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void key0_callbackFunction(struct my_key *key);
void key1_callbackFunction(struct my_key *key);
menu_t *menuInit(void);
void menuPrint(void);
void infoGroupFunction(void);
void nullFunction(void);
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
  static struct my_key key0 = {0}, key1 = {0};

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
  MX_I2C1_Init();
  MX_USART1_UART_Init();
  MX_TIM6_Init();
  /* USER CODE BEGIN 2 */

  OLED_Init();
  DHT11_Init();
  bh1750_config();
  // a structure which will contain all the data for one display
  u8g2_Setup_ssd1306_i2c_128x64_noname_1(&u8g2, U8G2_R0, u8x8_byte_i2c, u8x8_gpio_and_delay); // init u8g2 structure
  u8g2_InitDisplay(&u8g2);                                                                    // send init sequence to the display, display is in sleep mode after this,
  u8g2_SetPowerSave(&u8g2, 0);

  key0.cb = key0_callbackFunction;
  key1.cb = key1_callbackFunction;
  my_button_register(&key0);
  my_button_register(&key1);

  menu_t *menuHeadNode = menuInit();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    // int i = 10;
    // u8g2_FirstPage(&u8g2);
    // do
    // {
    // dataDisplay(&u8g2);

    // } while (u8g2_NextPage(&u8g2));
    // keyScan();

    key_scan_v2();
    // menuPrint();
    u8g2_FirstPage(&u8g2);
    do
    {

      menu_oled(sys_info.current_pointer);
    } while (u8g2_NextPage(&u8g2));
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

  /** Initializes the RCC Oscillators according to the specified parameters
   * in the RCC_OscInitTypeDef structure.
   */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
   */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void key0_callbackFunction(struct my_key *key)
{
  switch (key->event)
  {
  case CLICK: // 下
    usart_print(&huart1, "key0单击\r\n");
    if (sys_info.current_pointer->sibling_next != NULL)
      sys_info.current_pointer = sys_info.current_pointer->sibling_next;
    break;
  case DOUBLE_CLICK:
    usart_print(&huart1, "key0双击\r\n");
    break;
  case LONG_CLICK:
    usart_print(&huart1, "key0长按\r\n");
    break;
  default:
    break;
  }
}

void key1_callbackFunction(struct my_key *key)
{
  switch (key->event)
  {
  case CLICK: // 上
    usart_print(&huart1, "key1单击\r\n");
    if (sys_info.current_pointer->sibling_last != NULL)
      sys_info.current_pointer = sys_info.current_pointer->sibling_last;
    break;
  case DOUBLE_CLICK: // 确认，进入 或 触发函数
    usart_print(&huart1, "key1双击\r\n");
    if (sys_info.current_pointer->child != NULL)
      sys_info.current_pointer = sys_info.current_pointer->child;
    else
      sys_info.current_pointer->itemFunction();
    break;
  case LONG_CLICK: // 取消，返回
    usart_print(&huart1, "key1长按\r\n");
    if (sys_info.current_pointer->parent != NULL)
      sys_info.current_pointer = sys_info.current_pointer->parent;
    break;
  default:
    break;
  }
}

menu_t *menuInit(void)
{
  menu_t *headNode = NULL;
  headNode = add_sibling_item(headNode, nullFunction, "添加头节点");

  menu_t *first = add_child_item(headNode, "item1", nullFunction, 0);
  add_sibling_item(first, nullFunction, "item2");
  add_sibling_item(first, nullFunction, "item3");

  menu_t *second = add_child_item(first, "info", infoGroupFunction, 0);
  add_sibling_item(second, nullFunction, "system");
  add_sibling_item(second, nullFunction, "justice");

  sys_info.current_pointer = first;
  return headNode;
}

void menuPrint(void)
{
  menu_t *tPointer = sys_info.current_pointer;
  while (tPointer->sibling_last != NULL)
  {
    tPointer = tPointer->sibling_last;
  }

  usart_print(&huart1, "\r\n*********************************************************\r\n");
  usart_print(&huart1, "\r\n");
  usart_print(&huart1, "\r\n");
  do
  {
    if (sys_info.current_pointer == tPointer)
      usart_print(&huart1, ">\t%s\r\n", tPointer->item_name);
    else
      usart_print(&huart1, "\t%s\r\n", tPointer->item_name);
    tPointer = tPointer->sibling_next;

  } while (tPointer != NULL);
  usart_print(&huart1, "\r\n");
  usart_print(&huart1, "\r\n*********************************************************\r\n");
}

void menuOLED(void)
{
  menu_t *p = sys_info.current_pointer;
  while (p->sibling_last != NULL)
  {
    p = p->sibling_last;
  }
}

void infoGroupFunction(void)
{
  DHT11_Data_TypeDef DHT11Data;
  uint16_t light = 0;

  while (HAL_GPIO_ReadPin(KEY0_GPIO_Port, KEY0_Pin) == SET)
  {
    if (DHT11_Read_TempAndHumidity(&DHT11Data) != SUCCESS)
      usart_print(&huart1, "failed!");
    light = Get_BH1750_Value();

    u8g2_FirstPage(&u8g2);
    do
    {
      char strBuff[128];
      u8g2_ClearBuffer(&u8g2);
      u8g2_SetFont(&u8g2, u8g2_font_6x10_tf);
      sprintf(strBuff, "tempture: %d.%d °C", DHT11Data.temp_int, DHT11Data.temp_deci);
      u8g2_DrawUTF8(&u8g2, 1, 15, strBuff);
      sprintf(strBuff, "humidity: %d.%d %%RH", DHT11Data.humi_int, DHT11Data.humi_deci);
      u8g2_DrawUTF8(&u8g2, 1, 25, strBuff);
      sprintf(strBuff, "illumination: %d lx", light);
      u8g2_DrawUTF8(&u8g2, 1, 35, strBuff);
      u8g2_SendBuffer(&u8g2);
    } while (u8g2_NextPage(&u8g2));
  }
}

void nullFunction(void)
{
  return;
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

#ifdef USE_FULL_ASSERT
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
