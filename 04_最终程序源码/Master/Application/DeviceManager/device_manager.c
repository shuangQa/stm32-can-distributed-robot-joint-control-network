//
// Created by 双 on 2026/8/26.
//

#include "device_manager.h"

#include <stdio.h>

#include "protocol.h"

#include "app.h"
#include "stm32f1xx_hal.h"

Device_t g_device_table[MAX_DEVICE_NUM];

/*根据设备ID获取在注册表中的位置*/
Device_t* DeviceManager_FindDevice(uint8_t node_id)
{
    for(int i = 0; i < MAX_DEVICE_NUM; i++)
    {
        if(g_device_table[i].online||g_device_table[i].node_id == node_id)
        {
            return &g_device_table[i];
        }
    }
    return NULL;
}

/*将注册报文填入设备表的靠前的空余位*/
void DeviceManager_Register(const Register_Msg_t *msg)
{
    for(int i=0;i<MAX_DEVICE_NUM;i++)
    {
        if(g_device_table[i].online == 0)
        {
            g_device_table[i].node_id = msg->node_id;
            g_device_table[i].node_type = msg->node_type;
            g_device_table[i].online = 1;

            break;
        }
    }
}

/*心跳信息更新到设备表的函数*/
void DeviceManager_UpdateHeartbeat(uint8_t node_id)
{
    Device_t* dev=DeviceManager_FindDevice(node_id);
    if (dev!=NULL)
    {
          dev->last_heartbeat=HAL_GetTick();
    }
}

/*离线检测*/
void DeviceManager_CheckOffline()
{
    for(int i=0;i<MAX_DEVICE_NUM;i++)
    {
        if(g_device_table[i].online)
        {
            if(HAL_GetTick()-g_device_table[i].last_heartbeat>DEVICE_OFFLINE_TIMEOUT)
            {
                g_device_table[i].online=0;
                g_device_table[i].state= NODE_STATE_OFFLINE;
                Ack_Msg_t ack_msg;
                ack_msg.node_id=g_device_table[i].node_id;
                ack_msg.acked_id=CAN_ID_HEARTBEAT;

                Protocol_SendAck(&ack_msg);

            }
        }
    }
}

/*更新状态信息函数*/
void DeviceManager_UpdateState(const Status_Msg_t* state)
{
    Device_t* dev=DeviceManager_FindDevice(state->node_id);
    if (dev!=NULL)
    {
        dev->actual_rpm=state->actual_rpm;
        dev->pwm=state->pwm;
        dev->state=state->state;
        dev->fault_code=state->fault_code;
    }
}