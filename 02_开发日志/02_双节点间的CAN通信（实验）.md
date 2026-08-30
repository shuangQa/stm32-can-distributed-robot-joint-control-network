# 02\_双节点间的CAN通信（实验）

# 程序源码

## Master的main\.c

```C
*/* USER CODE BEGIN Header */*
*/***
*  *******************************************************************************
*  * @file           : main.c*
*  * @brief          : Main program body*
*  *******************************************************************************
*  * @attention*
*  **
*  * Copyright (c) 2026 STMicroelectronics.*
*  * All rights reserved.*
*  **
*  * This software is licensed under terms that can be found in the LICENSE file*
*  * in the root directory of this software component.*
*  * If no LICENSE file comes with this software, it is provided AS-IS.*
*  **
*  *******************************************************************************
*  */*
*/* USER CODE END Header */*
*/* Includes ------------------------------------------------------------------*/*
#include "main.h"

#include <string.h>

#include "can.h"
#include "gpio.h"

*/* Private includes ----------------------------------------------------------*/*
*/* USER CODE BEGIN Includes */*

*/* USER CODE END Includes */*

*/* Private typedef -----------------------------------------------------------*/*
*/* USER CODE BEGIN PTD */*

*/* USER CODE END PTD */*

*/* Private define ------------------------------------------------------------*/*
*/* USER CODE BEGIN PD */*

*/* USER CODE END PD */*

*/* Private macro -------------------------------------------------------------*/*
*/* USER CODE BEGIN PM */*

*/* USER CODE END PM */*

*/* Private variables ---------------------------------------------------------*/*

*/* USER CODE BEGIN PV */*

CAN_FilterTypeDef  sFilterConfig;   *//创建过滤器配置结构体的变量*
CAN_TxHeaderTypeDef TxHeader;   *//创建发送报文的头部信息（仲裁段：ID、控制段：数据长度、数据段）的结构体变量*
uint8_t TxData[8] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};   *//发送的数据内容*
uint32_t TxMailbox;

*/* USER CODE END PV */*

*/* Private function prototypes -----------------------------------------------*/*
void SystemClock_Config(void);
*/* USER CODE BEGIN PFP */*

*/* USER CODE END PFP */*

*/* Private user code ---------------------------------------------------------*/*
*/* USER CODE BEGIN 0 */*

*/* USER CODE END 0 */*

*/***
*  * @brief  The application entry point.*
*  * @retval int*
*  */*
int main(void)
{

  */* USER CODE BEGIN 1 */*

*  /* USER CODE END 1 */*

*  /* MCU Configuration--------------------------------------------------------*/*

*  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */*
*  *HAL_Init();

  */* USER CODE BEGIN Init */*

*  /* USER CODE END Init */*

*  /* Configure the system clock */*
*  *SystemClock_Config();

  */* USER CODE BEGIN SysInit */*

*  /* USER CODE END SysInit */*

*  /* Initialize all configured peripherals */*
*  *MX_GPIO_Init();
  MX_CAN_Init();
  */* USER CODE BEGIN 2 */*

*  //配置过滤器（接收准备）*

*  *sFilterConfig.FilterActivation = *ENABLE*;    *//开始配置过滤器，对结构体内各参数赋值（当前，接收所有ID）*
*  *sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
  sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
  sFilterConfig.FilterIdHigh = 0x0000;
  sFilterConfig.FilterIdLow = 0x0000;
  sFilterConfig.FilterMaskIdHigh = 0x0000;
  sFilterConfig.FilterMaskIdLow = 0x0000;
  sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;
  sFilterConfig.FilterBank = 0;
  sFilterConfig.SlaveStartFilterBank = 0;

  *//使用函数将配置结构体应用*
*  *if(HAL_CAN_ConfigFilter(&hcan,&sFilterConfig)!=*HAL_OK*)
  {
    Error_Handler();    *//错误跳转*
*  *}

  *//启动CAN外设*
*  *if (HAL_CAN_Start(&hcan)!=*HAL_OK*)
  {
    Error_Handler();
  }

  *//使能CAN接收中断*
*  *if(HAL_CAN_ActivateNotification(&hcan,CAN_IT_RX_FIFO0_MSG_PENDING)!=*HAL_OK*)
  {
    Error_Handler();
  }

  */* USER CODE END 2 */*

*  /* Infinite loop */*
*  /* USER CODE BEGIN WHILE */*
*  *while (1)
  {
    */* USER CODE END WHILE */*

*    /* USER CODE BEGIN 3 */*

*    //设置发送消息的结构体*
*    *memset(&TxHeader, 0, sizeof(TxHeader));

    TxHeader.StdId = 0x123;
    TxHeader.IDE   = CAN_ID_STD;
    TxHeader.RTR   = CAN_RTR_DATA;
    TxHeader.DLC   = 8;

    *//发送报文*
*    *if (HAL_CAN_AddTxMessage(&hcan, &TxHeader, TxData, &TxMailbox) != *HAL_OK*)
    {
      Error_Handler();
    }

    *//延时1s*
*    *HAL_Delay(1000);
    
  }
  */* USER CODE END 3 */*
}

*/***
*  * @brief System Clock Configuration*
*  * @retval None*
*  */*
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  */** Initializes the RCC Oscillators according to the specified parameters*
*  * in the RCC_OscInitTypeDef structure.*
*  */*
*  *RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != *HAL_OK*)
  {
    Error_Handler();
  }

  */** Initializes the CPU, AHB and APB buses clocks*
*  */*
*  *RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != *HAL_OK*)
  {
    Error_Handler();
  }
}

*/* USER CODE BEGIN 4 */*

*/* USER CODE END 4 */*

*/***
*  * @brief  This function is executed in case of error occurrence.*
*  * @retval None*
*  */*
void Error_Handler(void)
{
  */* USER CODE BEGIN Error_Handler_Debug */*
*  /* User can add his own implementation to report the HAL error return state */*
*  *__disable_irq();
  while (1)
  {
  }
  */* USER CODE END Error_Handler_Debug */*
}
#ifdef USE_FULL_ASSERT
*/***
*  * @brief  Reports the name of the source file and the source line number*
*  *         where the assert_param error has occurred.*
*  * @param  file: pointer to the source file name*
*  * @param  line: assert_param error line source number*
*  * @retval None*
*  */*
void assert_failed(uint8_t *file, uint32_t line)
{
  */* USER CODE BEGIN 6 */*
*  /* User can add his own implementation to report the file name and line number,*
*     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */*
*  /* USER CODE END 6 */*
}
#endif */* USE_FULL_ASSERT */*
```

## JointNode的main\.c

```Markdown
*/* USER CODE BEGIN Header */*
*/***
*  *******************************************************************************
*  * @file           : main.c*
*  * @brief          : Main program body*
*  *******************************************************************************
*  * @attention*
*  **
*  * Copyright (c) 2026 STMicroelectronics.*
*  * All rights reserved.*
*  **
*  * This software is licensed under terms that can be found in the LICENSE file*
*  * in the root directory of this software component.*
*  * If no LICENSE file comes with this software, it is provided AS-IS.*
*  **
*  *******************************************************************************
*  */*
*/* USER CODE END Header */*
*/* Includes ------------------------------------------------------------------*/*
#include "main.h"
#include "can.h"
#include "gpio.h"

*/* Private includes ----------------------------------------------------------*/*
*/* USER CODE BEGIN Includes */*

*/* USER CODE END Includes */*

*/* Private typedef -----------------------------------------------------------*/*
*/* USER CODE BEGIN PTD */*

*/* USER CODE END PTD */*

*/* Private define ------------------------------------------------------------*/*
*/* USER CODE BEGIN PD */*

*/* USER CODE END PD */*

*/* Private macro -------------------------------------------------------------*/*
*/* USER CODE BEGIN PM */*

*/* USER CODE END PM */*

*/* Private variables ---------------------------------------------------------*/*

*/* USER CODE BEGIN PV */*

*/* USER CODE END PV */*

*/* Private function prototypes -----------------------------------------------*/*
void SystemClock_Config(void);
*/* USER CODE BEGIN PFP */*

*/* USER CODE END PFP */*

*/* Private user code ---------------------------------------------------------*/*
*/* USER CODE BEGIN 0 */*

*/* USER CODE END 0 */*

*/***
*  * @brief  The application entry point.*
*  * @retval int*
*  */*
int main(void)
{

  */* USER CODE BEGIN 1 */*

*  /* USER CODE END 1 */*

*  /* MCU Configuration--------------------------------------------------------*/*

*  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */*
*  *HAL_Init();

  */* USER CODE BEGIN Init */*

*  /* USER CODE END Init */*

*  /* Configure the system clock */*
*  *SystemClock_Config();

  */* USER CODE BEGIN SysInit */*

*  /* USER CODE END SysInit */*

*  /* Initialize all configured peripherals */*
*  *MX_GPIO_Init();
  MX_CAN_Init();
  */* USER CODE BEGIN 2 */*

*  //配置过滤器（接收准备)*
*  *CAN_FilterTypeDef  sFilterConfig;   *//创建过滤器配置结构体的变量*

*  *sFilterConfig.FilterActivation = *ENABLE*;    *//开始配置过滤器，对结构体内各参数赋值（当前，接收所有ID）*
*  *sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
  sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
  sFilterConfig.FilterIdHigh = 0x0000;
  sFilterConfig.FilterIdLow = 0x0000;
  sFilterConfig.FilterMaskIdHigh = 0x0000;
  sFilterConfig.FilterMaskIdLow = 0x0000;
  sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;
  sFilterConfig.FilterBank = 0;
  sFilterConfig.SlaveStartFilterBank = 0;

  *//使用函数将配置结构体应用*
*  *if(HAL_CAN_ConfigFilter(&hcan,&sFilterConfig)!=*HAL_OK*)
  {
    Error_Handler();    *//错误跳转*
*  *}

  *//启动CAN外设*
*  *if (HAL_CAN_Start(&hcan)!=*HAL_OK*)
  {
    Error_Handler();
  }

  *//使能CAN接收中断*
*  *if(HAL_CAN_ActivateNotification(&hcan,CAN_IT_RX_FIFO0_MSG_PENDING)!=*HAL_OK*)
  {
    Error_Handler();
  }

  */* USER CODE END 2 */*

*  /* Infinite loop */*
*  /* USER CODE BEGIN WHILE */*
*  *while (1)
  {
    */* USER CODE END WHILE */*

*    /* USER CODE BEGIN 3 */*
*  *}
  */* USER CODE END 3 */*
}

*/***
*  * @brief System Clock Configuration*
*  * @retval None*
*  */*
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  */** Initializes the RCC Oscillators according to the specified parameters*
*  * in the RCC_OscInitTypeDef structure.*
*  */*
*  *RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != *HAL_OK*)
  {
    Error_Handler();
  }

  */** Initializes the CPU, AHB and APB buses clocks*
*  */*
*  *RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != *HAL_OK*)
  {
    Error_Handler();
  }
}

*/* USER CODE BEGIN 4 */*

*//重新定义接收中断回调函数*


void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
  while (1);
}

*/* USER CODE END 4 */*

*/***
*  * @brief  This function is executed in case of error occurrence.*
*  * @retval None*
*  */*
void Error_Handler(void)
{
  */* USER CODE BEGIN Error_Handler_Debug */*
*  /* User can add his own implementation to report the HAL error return state */*
*  *__disable_irq();
  while (1)
  {
  }
  */* USER CODE END Error_Handler_Debug */*
}
#ifdef USE_FULL_ASSERT
*/***
*  * @brief  Reports the name of the source file and the source line number*
*  *         where the assert_param error has occurred.*
*  * @param  file: pointer to the source file name*
*  * @param  line: assert_param error line source number*
*  * @retval None*
*  */*
void assert_failed(uint8_t *file, uint32_t line)
{
  */* USER CODE BEGIN 6 */*
*  /* User can add his own implementation to report the file name and line number,*
*     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */*
*  /* USER CODE END 6 */*
}
#endif */* USE_FULL_ASSERT */*
```

# 硬件连接（如图）

![Image](https://internal-api-drive-stream.feishu.cn/space/api/box/stream/download/authcode/?code=ZjczMWNlODlmYjU3NzJmYTAyZTA4MTg2MDQ3YzYwNmVfYzI2MGFkMGYzMWFmZDgxZmM0ZDg2YTY4N2RmNjY2NTZfSUQ6NzY3ODkxNjY1MDQ2MDA3MjkzNl8xNzg3ODg3MzMxOjE3ODc5NzM3MzFfVjM)

# 调试记录

## 问题描述

STLINK连接joint节点调试，发现始终停在主函数的while循环，没进入回调本来设置的死循环

## 调试过程

在回调内：

```C
volatile uint32_t g_can_cnt = 0;

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
  g_can_cnt++;
}
```

查变量g\_can\_cnt始终是0，所以根本没接收到报文

stlink连接master主控调试，发现一点击调试没几秒就调试器断开，然后试了一下烧录程序也烧录不了

拿万用表测量，CAN收发器的VCC和GND之间是5V，没什么问题，把CAN相关断掉，仅保留主控，还是不能烧录

最后发现是另一个窗口的JOINT节点的调试未关闭，关闭后，能调试master了



进入master调试，由于master编程时就写了很多方便调试的程序，所以一开始调试，再暂停，就发现由发送报文函数进入了错误处理函数

![Image](https://internal-api-drive-stream.feishu.cn/space/api/box/stream/download/authcode/?code=M2Q1NjJmYTAwMmMxOTIzNWY2NmFjMGFlM2EwZDMyYWRfODMyY2M5NWFhMmUyM2U3MzdhMzY1MTRjMDBjZWY3ZDlfSUQ6NzY3ODkxNjY0NzI0MzU4Mjc0N18xNzg3ODg3MzMxOjE3ODc5NzM3MzFfVjM)

```C
if (HAL_CAN_AddTxMessage(&hcan, &TxHeader, TxData, &TxMailbox) != HAL_OK)
    {
       
      uint32_t error = HAL_CAN_GetError(&hcan
      );
      while(1)
      {
      };
    }
```

错误码2097152，转换十六进制后对应CAN未初始化

但是，如果初始化错误，前面的启动函数就该跳转到ERROR\_Handler\(\)了

所以这个错误码可能是历史状态。

重新上电

还是2097152，但是我发现，第一次发送函数好像可以执行，在之后的循环中，才会出错

所以，在发送函数之前打断点，逐步执行

果然没有进入死循环，执行了发送函数之后进入延时1s

再次步进，然后不断查询状态

很多现象，前几次发送函数返回HAL\_OK,循环时间久了就返回错误

但是不知道为什么，不断地改不断的找

然后做主控单独的回环测试，因为之前在节点里设置LED翻转，正常运行

且万用表监测，CAN\_H和CAN\_L正常是之间0电压，然后断电时监测有终端电阻

且主控和节点之间的CAN\_H和CAN\_H，CAN\_L和CAN\_L之间短路蜂鸣器响，也没问题

接下来单独监测主控回环测试所以

不断地查错误码，但是不知道原因

后来感觉这个和昨天的回环测试情况很像

按照昨天的做法找到时间片分配如图

![Image](https://internal-api-drive-stream.feishu.cn/space/api/box/stream/download/authcode/?code=YWQwYTI1MmNhNWY5OTRjZDAwN2FiZWUxNGRmNjM3YmRfMzUyMzU2OWUxNzgzZWZlZjAzM2NmNGM2ZDE5MmU4NDNfSUQ6NzY3ODkxNjY0ODgyMDU1ODgxNl8xNzg3ODg3MzMxOjE3ODc5NzM3MzFfVjM)

先是直接用昨天代码，回环模式改成正常模式

发送失败

想起来节点波特率不同

将节点时间片也改成如图，波特率相同

发送成功，实现，隔着500ms主控发送一次，节点小灯翻转

## 发现问题

原本的时间片配置是：

![Image](https://internal-api-drive-stream.feishu.cn/space/api/box/stream/download/authcode/?code=OTc4MTM2YTYxMGRlYWI5Nzc1NGI1ZGZhZjM5OTYwNGZfODVjYTczNTBlNjcwMDViMzJiMzQ3MTcwMDk5M2NlNjlfSUQ6NzY3ODkxNjY0NzY3MTYzMTEzNF8xNzg3ODg3MzMxOjE3ODc5NzM3MzFfVjM)

后面改成：

![Image](https://internal-api-drive-stream.feishu.cn/space/api/box/stream/download/authcode/?code=Mzc4NDM3YjE1MmVjZDcyOWQ0M2EyMjkxMGUzMTc0OGZfNmJlOTY3ZjhmNmY0MzIwOWFhNWU4MmM1N2EwODMyNGFfSUQ6NzY3ODkxNjY1MDUzMTUzOTkwNl8xNzg3ODg3MzMxOjE3ODc5NzM3MzFfVjM)

就发送成功了。

## 原因分析

失败配置：BS1=1

成功配置:BS1=13

1. 采样点过早，**CAN推荐的采样点是：75%\~90%（推荐87\.5%），过低的采样点会导致总线信号尚未稳定时就被采样，极易产生位错误。**

采样点计算公式：\(1\+BS1\)/\(1\+BS1\+BS2\)

```C
失败配置下的采    样点：
(1+BS1)/(1+BS1+BS2)

=2/4

=50%
```

2. 位时间过短

BS1=1 时，总时间量子数 = 1\+1\+BS2。如果 BS2 也很小（比如 1），总时间量子仅为 3，低于多数 CAN 控制器要求的最小时间量子数（通常≥8）。

STM32 的 bxCAN 要求位时间至少为 4 个 tq（实际手册可能建议 ≥8），否则硬件可能无法正常工作。

3. 控制器内部状态错误

不合理的位时序可能导致 CAN 控制器内部错误计数器增加，最终进入 Bus\-Off 或 Error Passive 状态，发送被禁止。

## 经验积累

### 正确配置BS1/BS2的准则

- 采样点位置：建议设置在 75%\~87\.5% 之间。公式：

- \(1 \+ BS1\) / \(1 \+ BS1 \+ BS2\) ≈ 0\.75 \~ 0\.875

- BS1 ≥ BS2：通常 BS1 应大于等于 BS2，以保证足够的传播延时补偿。

- 总时间量子数：建议 ≥8（例如 1\+13\+2=16，1\+11\+4=16 等）。

- 波特率计算：波特率 = PCLK1 / \(Prescaler × \(1 \+ BS1 \+ BS2\)\)

- 确保主控和节点使用相同的波特率。

### 常见波特率配置示例（以 STM32F103，PCLK1=36MHz 为例）

![Image](https://internal-api-drive-stream.feishu.cn/space/api/box/stream/download/authcode/?code=OGFlMjY1OWUzNGNhYzQ1MzI4OTY0MjQ4ODMxMDNiZDNfNDM5MTc2NDVjNWE0ZWIzYTFlOWM2MWZlMTAyYzViYjNfSUQ6NzY3ODkxNjY0OTg3NzU4ODk0OV8xNzg3ODg3MzMxOjE3ODc5NzM3MzFfVjM)



