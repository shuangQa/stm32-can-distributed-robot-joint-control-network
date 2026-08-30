//
// Created by 双 on 2026/8/27.
//

#ifndef JOINTNODE_JOINT_CONTROL_H
#define JOINTNODE_JOINT_CONTROL_H

#include <stdint.h>

#include "protocol.h"

typedef struct
{
    int16_t target_rpm;    //目标转速
    int16_t actual_rpm;    //当前/实际转速
    uint8_t enable;        //使能位
    uint8_t state;         //节点状态
}JointControl_t;

extern JointControl_t g_joint;

/*更新控制要求函数*/
void Motor_UpdateControl(const Control_Msg_t *ctrl);

/*电机执行函数*/
void Motor_Task();

/*电机急停函数*/
void Motor_Stop();

#endif //JOINTNODE_JOINT_CONTROL_H
