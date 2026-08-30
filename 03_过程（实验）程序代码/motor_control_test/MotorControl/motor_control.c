//
// Created by 双 on 2026/8/27.
//

#include "motor_control.h"

#include <stddef.h>

#include "protocol.h"
JointControl_t g_joint;

/*更新控制要求函数*/
void Motor_UpdateControl(const Control_Msg_t *ctrl)
{
    g_joint.target_rpm = ctrl->target_rpm;
    g_joint.enable = ctrl->enable;
}

/*电机执行函数*/
void Motor_Task()
{
    if(g_joint.enable)
    {
        //以后PID控制
    }
    else
    {
        //停止电机
    }
}

/*电机急停函数*/
void Motor_Stop()
{

}