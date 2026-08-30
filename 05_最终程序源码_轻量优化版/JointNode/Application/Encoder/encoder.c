//
// Created by 双 on 2026/8/29.
//

#include "stm32f1xx_hal.h"

#include "stm32f1xx_hal_tim.h"

#include "encoder.h"

#include "tim.h"

#define ENCODER_CPR 7436

static int16_t encoder_cnt = 0;
static int16_t last_cnt=0;
static int16_t delta_cnt=0;
static float rpm=0.0f;

/*初始化或启动编码器*/
void Encoder_Init(void)
{
    HAL_TIM_Encoder_Start(&htim2, TIM_CHANNEL_ALL);
}

/*更新编码器计数器值和增量*/
void Encoder_Update(void)
{
    encoder_cnt=__HAL_TIM_GET_COUNTER(&htim2);
    delta_cnt = (int16_t)(encoder_cnt - last_cnt);
   // delta_cnt=encoder_cnt-last_cnt;    
    last_cnt=encoder_cnt;       //防止计数器溢出之后计算结果爆掉，虽然我现在脑子疼没去细究怎么防止的
    rpm=delta_cnt*600.0f/ENCODER_CPR;
}

/*返回计数器值*/
int16_t Encoder_GetCount(void)
{
    return encoder_cnt;
}

/*返回增量值*/
int16_t Encoder_GetDelta(void)
{
    return delta_cnt;
}

/*返回转速*/
float Encoder_GetRpm(void)
{
    return rpm;
}