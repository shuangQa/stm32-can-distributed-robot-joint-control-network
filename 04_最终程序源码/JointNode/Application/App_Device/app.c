//
// Created by 双 on 2026/8/26.
//

#include "app.h"

#include "motor_control.h"
#include "protocol.h"
#include "state_machine.h"

/*此设备节点ID*/
uint8_t Local_NodeID=NODE_ID_JOINT1;

/*此设备节点类型*/
uint16_t Local_NodeType=NODE_TYPE_MOTOR;

/*执行注册函数*/
void App_Register(void)
{
    Register_Msg_t registerMsg;
    registerMsg.node_id=Local_NodeID;
    registerMsg.node_type=Local_NodeType;

    Protocol_SendRegister(&registerMsg);
}

/*发送心跳函数*/
void App_Heartbeat(void)
{
    Heartbeat_Msg_t heartbeatMsg;
    heartbeatMsg.node_id=Local_NodeID;
    heartbeatMsg.state=NODE_STATE_NORMAL;

    Protocol_SendHeartbeat(&heartbeatMsg);
}

/*处理控制报文函数*/
void App_HandleControl(const Control_Msg_t *ctrl)
{
    Motor_UpdateControl(ctrl);

    Ack_Msg_t ack;

    ack.node_id = Local_NodeID;
    ack.acked_id = CAN_ID_CONTROL;
    ack.seq_num = ctrl->seq_num;
    ack.result=1;
    Protocol_SendAck(&ack);
}

/*获取状态信息函数*/
void App_GetStatus(Status_Msg_t* status)
{
    status->node_id=Local_NodeID;
    status->actual_rpm=g_joint.actual_rpm;
    status->pwm=g_pwm;
    status->state=NODE_STATE_NORMAL;
    status->fault_code=1;

    if (status->state==NODE_STATE_FAULT)
    {
        StateMachine_SetState(NODE_FAULT);
    }
}

/*发送状态信息*/
void App_Status()
{
    Status_Msg_t status;

    App_GetStatus(&status);

    Protocol_SendStatus(&status);
}

/*处理应答信号*/
void App_HandleAck(const Ack_Msg_t *ack)
{
    if (ack->acked_id == CAN_ID_REGISTER)
    {
        StateMachine_SetState(NODE_ONLINE);
    }else if (ack->acked_id==CAN_ID_HEARTBEAT)
    {
        if (ack->node_id==Local_NodeID)
        {
            StateMachine_SetState(NODE_OFFLINE);
        }
    }
}