//
// Created by 双 on 2026/8/28.
//

#include "state_machine.h"

#include "app.h"
#include "encoder.h"
#include "motor_control.h"
#include "stm32f1xx_hal.h"

NodeState_t g_state = NODE_INIT;

void StateMachine_SetState(NodeState_t state)
{
    if (g_state!=state)
    {
        g_state = state;
        //而且这里可以添加断点来查看状态切换
    }

}

void StateMachine_Update()
{
    static uint32_t init_time = 0;
    static uint32_t last_register_time = 0;
    static uint32_t heartbeat_tick = 0;
    static uint32_t status_tick = 0;

    switch (g_state)
    {
    case NODE_INIT:
        {
            if (init_time==0)
            {
                init_time = HAL_GetTick();
            }
            if (HAL_GetTick() - init_time > 10000)
            {
                 StateMachine_SetState(NODE_REGISTERING);
            }
            break;
        }
    case NODE_REGISTERING:
        {
            if(HAL_GetTick() - last_register_time > 1000)
            {
                App_Register();//接收到应答信号时，回调函数跳转到协议解析后会将状态改为NODE_ONLINE
                last_register_time = HAL_GetTick();
            }

           break;
        }
    case NODE_ONLINE:
        {
            if (HAL_GetTick() - heartbeat_tick > 1000)
            {
                App_Heartbeat();   //主控的超时检测发现超时后，发送应答信号告知节点ID，经过判断在回调函数内会将对应的节点进入OFFLINE状态
                heartbeat_tick = HAL_GetTick();
            }
            if (HAL_GetTick() - status_tick > 500)
            {
                App_Status();   //状态函数在获取状态信息后，如果发现状态是FAULT，会进入FAULT
                status_tick = HAL_GetTick();
            }

            Encoder_Update();

            Motor_Task(&pid);

            break;
        }
    case NODE_OFFLINE:
        {
            if(HAL_GetTick() - last_register_time > 1000)
            {
                App_Register();//接收到应答信号时，回调函数跳转到协议解析后会将状态改为NODE_ONLINE
                last_register_time = HAL_GetTick();
            }
            break;
        }
    case NODE_FAULT:
        {
            Motor_Stop();

            /*之后会封装一个函数*/
            Status_Msg_t status;
            App_GetStatus(&status);
            if (status.state!=NODE_FAULT)
            {
                StateMachine_SetState(NODE_ONLINE);
            }

            break;
        }
    default:
        {
            break;
        }
    }
}
