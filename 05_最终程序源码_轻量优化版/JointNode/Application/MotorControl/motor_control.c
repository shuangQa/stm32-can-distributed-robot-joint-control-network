//
// Created by 双 on 2026/8/27.
//

#include "motor_control.h"

#include "encoder.h"
#include "stm32f1xx_hal.h"
#include "protocol.h"
#include "stm32f1xx_hal_tim.h"
#include "tim.h"
#include "../PID/pid.h"

JointControl_t g_joint;
float g_pwm=0;

/*电机初始化（启动PWM输出）,电机驱动使能*/
void Motor_Init(void)
{
    HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_1);
    HAL_GPIO_WritePin(STBY_GPIO_Port, STBY_Pin, GPIO_PIN_SET);
}

/*电机正转*/
void Motor_Forward(void)
{
    HAL_GPIO_WritePin(AIN1_GPIO_Port, AIN1_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(AIN2_GPIO_Port, AIN2_Pin, GPIO_PIN_RESET);
}

/*电机反转*/
void Motor_Backward(void)
{
    HAL_GPIO_WritePin(AIN1_GPIO_Port, AIN1_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(AIN2_GPIO_Port, AIN2_Pin, GPIO_PIN_SET);
}

/*电机停止（PWM为0）*/
void Motor_Stop(void)
{
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, 0);
}

/*设置电机PWM*/
void Motor_SetPWM(float pwm)
{
    if (pwm < 0.0f)
    {
        pwm = 0.0f;
    }
    else if (pwm > 95.0f)
    {
        pwm = 95.0f;
    }

    g_pwm=pwm;
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, (uint32_t)(pwm*10.0f));
}

/*获取PWM值*/
float Motor_GetPWM(void)
{
    return g_pwm;
}

/*更新控制要求函数*/
void Motor_UpdateControl(const Control_Msg_t *ctrl)
{
    if (ctrl == 0)
    {
        return;
    }

    g_joint.target_rpm = ctrl->target_rpm;
    g_joint.enable = ctrl->enable;
}


/*电机执行函数*/
void Motor_Task(PID_t *pid)
{
    if (pid == 0 || g_joint.enable == MOTOR_DISABLE || g_joint.target_rpm <= 0.0f)
    {
        Motor_Stop();
        PID_Reset(pid);
        g_pwm = 0.0f;
        g_joint.actual_rpm = Encoder_GetRpm();
        return;
    }

    g_joint.actual_rpm=Encoder_GetRpm();
    g_pwm=PID_Calculate(pid,g_joint.target_rpm,g_joint.actual_rpm);
    Motor_Forward();
    Motor_SetPWM(g_pwm);
}

/*电机急停函数*/
void Motor_QuickStop()
{

}
