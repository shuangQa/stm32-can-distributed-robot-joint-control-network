//
// Created by 双 on 2026/8/26.
//

#include "app.h"

#include "protocol.h"

#include "device_manager.h"
#include "stm32f1xx_hal.h"

static uint8_t g_control_seq = 0;
static volatile uint8_t g_wait_ack_seq = 0;
static volatile uint8_t g_wait_ack = 0;

/*注册报文处理函数*/
void App_HandleRegister(const Register_Msg_t *msg)
{
    DeviceManager_Register(msg);

    Ack_Msg_t ack = {0};
    ack.node_id = NODE_ID_MASTER;
    ack.acked_id = CAN_ID_REGISTER;
    ack.seq_num = 0;     //后面给个取值方法需要
    ack.result = 1;

    Protocol_SendAck(&ack);
}

/*控制节点函数*/
uint8_t App_SendControlcmd(int16_t target_rpm,uint8_t node_id)
{
    Control_Msg_t ctrl = {0};
    ctrl.node_id = node_id;
    ctrl.target_rpm = target_rpm;
    ctrl.seq_num=g_control_seq++;
    ctrl.enable = 1;
    g_wait_ack_seq=ctrl.seq_num;
    g_wait_ack=1;

    if (!Protocol_SendControl(&ctrl))
    {
        g_wait_ack = 0;
        return 0;
    }

    uint32_t start_tick = HAL_GetTick();
    uint32_t last_retry_tick = start_tick;
    while (g_wait_ack)
    {
        uint32_t now = HAL_GetTick();
        if (now - start_tick > 500)
        {
            g_wait_ack = 0;
            return 0;
        }
        if (now - last_retry_tick > 100)
        {
            Protocol_SendControl(&ctrl);
            last_retry_tick = now;
        }
    }

    return 1;
}

/*处理设备应答报文的函数*/
void App_HandleAck(const Ack_Msg_t *msg)
{
    if (msg == 0)
    {
        return;
    }

    if (msg->acked_id==CAN_ID_CONTROL)
    {
        if (msg->seq_num==g_wait_ack_seq)
        {
            g_wait_ack=0;
        }
    }
    /*
     *应答逻辑,很大一部分，之后可能需要单独文件夹
     */
}

/*处理心跳报文的函数*/
void App_HandleHeartbeat(const Heartbeat_Msg_t *msg)
{
    DeviceManager_UpdateHeartbeat(msg->node_id);

     Ack_Msg_t ack = {0};
    ack.node_id = NODE_ID_MASTER;
    ack.acked_id = CAN_ID_HEARTBEAT;
    ack.seq_num = 0;
    ack.result = 1;

     Protocol_SendAck(&ack);
}

/*处理状态报文的函数*/
void App_HandleStatus(const Status_Msg_t *msg)
{
    DeviceManager_UpdateState(msg);

    Ack_Msg_t ack = {0};
    ack.node_id = NODE_ID_MASTER;
    ack.acked_id = CAN_ID_STATUS;
    ack.seq_num = 0;
    ack.result = 1;

    Protocol_SendAck(&ack);
}
