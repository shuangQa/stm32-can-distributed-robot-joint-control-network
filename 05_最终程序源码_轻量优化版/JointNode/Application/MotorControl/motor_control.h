//
// Created by 双 on 2026/8/27.
//

#ifndef JOINTNODE_JOINT_CONTROL_H
#define JOINTNODE_JOINT_CONTROL_H

#include <stdint.h>

#include "pid.h"
#include "protocol.h"

typedef struct
{
    float target_rpm;    //目标转速
    float actual_rpm;    //当前/实际转速
    uint8_t enable;        //使能位
    uint8_t state;         //节点状态
}JointControl_t;

extern JointControl_t g_joint;

extern float g_pwm;

/*电机初始化（启动PWM输出）,电机驱动使能*/
void Motor_Init(void);

/*电机正转*/
void Motor_Forward(void);

/*电机反转*/
void Motor_Backward(void);

/*电机停止（PWM为0）*/
void Motor_Stop(void);

/*设置电机PWM*/
void Motor_SetPWM(float pwm);

/*获取PWM值*/
float Motor_GetPWM(void);

/*更新控制要求函数*/
void Motor_UpdateControl(const Control_Msg_t *ctrl);

/*电机执行函数*/
void Motor_Task(PID_t *pid);

/*电机急停函数*/
void Motor_QuickStop(void);

#endif //JOINTNODE_JOINT_CONTROL_H
