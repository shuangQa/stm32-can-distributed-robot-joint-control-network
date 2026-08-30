//
// Created by 双 on 2026/8/26.
//

#include "protocol.h"

#include <string.h>

#include "app.h"
#include "../CAN/can_driver.h"

/*16位数据转为8位形式存储进报文*/
static void PutU16(uint8_t *buf,uint16_t val)
{
    buf[0]=(uint8_t)(val&0xFF);
    buf[1]=(uint8_t)(val>>8&0xFF);
}
static void PutI16(uint8_t *buf, int16_t val) {
    PutU16(buf, (uint16_t)val);
}

/*8位数据转为16位形式从报文中读取后存入结构体*/
static uint16_t GetU16(const uint8_t *buf) {
    return (uint16_t)(buf[0]) | ((uint16_t)buf[1] << 8);
}

static int16_t GetI16(const uint8_t *buf) {
    return (int16_t)GetU16(buf);
}

/* 辅助函数：将各种报文结构体转换为 8 字节数据数组 */
void Control_To_Data(const Control_Msg_t *msg, uint8_t *data)
{
    memset(data, 0, CAN_DATA_LEN);
    data[0] = msg->node_id;
    PutI16(&data[1], msg->target_rpm);
    data[3] = msg->enable;
    data[4] = msg->seq_num;
}

void Status_To_Data(const Status_Msg_t *msg, uint8_t *data)
{
    memset(data, 0, CAN_DATA_LEN);
    data[0] = msg->node_id;
    PutI16(&data[1], msg->actual_rpm);
    data[3] =msg->pwm;
    data[4]=msg->state;
    data[5]=msg->fault_code;
}

void Heartbeat_To_Data(const Heartbeat_Msg_t *msg, uint8_t *data)
{
    memset(data, 0, CAN_DATA_LEN);
    data[0] = msg->node_id;
    data[1] = msg->state;
}

void Register_To_Data(const Register_Msg_t *msg, uint8_t *data)
{
    memset(data, 0, CAN_DATA_LEN);
    data[0] = msg->node_id;
    PutU16(&data[1], msg->node_type);
}

void Ack_To_Data(const Ack_Msg_t *msg, uint8_t *data)
{
    memset(data, 0, CAN_DATA_LEN);
    data[0] = msg->node_id;
    PutU16(&data[1], msg->acked_id);
    data[3] = msg->seq_num;
    data[4] = msg->result;
}

/* 辅助函数：从 8 字节数据数组解析为各种报文结构体 */
void Data_To_Control(const uint8_t *data, Control_Msg_t *msg)
{
    msg->node_id = data[0];
    msg->target_rpm = GetI16(&data[1]);
    msg->enable = data[3];
    msg->seq_num = data[4];
}

void Data_To_Status(const uint8_t *data, Status_Msg_t *msg)
{
    msg->node_id = data[0];
    msg->actual_rpm = GetI16(&data[1]);
    msg->pwm = data[3];
    msg->state = data[4];
    msg->fault_code = data[5];
}

void Data_To_Heartbeat(const uint8_t *data, Heartbeat_Msg_t *msg)
{
    msg->node_id = data[0];
    msg->state = data[1];
}

void Data_To_Register(const uint8_t *data, Register_Msg_t *msg)
{
    msg->node_id = data[0];
    msg->node_type = GetU16(&data[1]);
}

void Data_To_Ack(const uint8_t *data, Ack_Msg_t *msg)
{
    msg->node_id = data[0];
    msg->acked_id = GetU16(&data[1]);
    msg->seq_num = data[3];
    msg->result = data[4];
}

/*各类型报文发送*/
uint8_t Protocol_SendControl(const Control_Msg_t *msg)
{
    CAN_Message_t can_msg;
    can_msg.id=CAN_ID_CONTROL;
    can_msg.length=CAN_DATA_LEN;
    Control_To_Data(msg,can_msg.data);
    return CAN_SendMessage(&can_msg);
}

uint8_t Protocol_SendStatus(const Status_Msg_t *msg)
{
    CAN_Message_t can_msg;
    can_msg.id=CAN_ID_STATUS;
    can_msg.length=CAN_DATA_LEN;
    Status_To_Data(msg,can_msg.data);
    return CAN_SendMessage(&can_msg);
}

uint8_t Protocol_SendHeartbeat(const Heartbeat_Msg_t *msg)
{
    CAN_Message_t can_msg;
    can_msg.id=CAN_ID_HEARTBEAT;
    can_msg.length=CAN_DATA_LEN;
    Heartbeat_To_Data(msg,can_msg.data);
    return CAN_SendMessage(&can_msg);
}

uint8_t Protocol_SendRegister(const Register_Msg_t *msg)
{
    CAN_Message_t can_msg;
    can_msg.id=CAN_ID_REGISTER;
    can_msg.length=CAN_DATA_LEN;
    Register_To_Data(msg,can_msg.data);
    return CAN_SendMessage(&can_msg);
}

uint8_t Protocol_SendAck(const Ack_Msg_t *msg)
{
    CAN_Message_t can_msg;
    can_msg.id=CAN_ID_ACK;
    can_msg.length=CAN_DATA_LEN;
    Ack_To_Data(msg,can_msg.data);
    return CAN_SendMessage(&can_msg);
}

/*报文接收后的分析函数（未引入设备管理）*/
uint8_t Protocol_Parse(const CAN_Message_t *msg)
{
    if(msg == NULL || msg->length > CAN_DATA_LEN) return 0;
    switch (msg->id)
    {
    case CAN_ID_CONTROL:
        {
            if (msg->length < 5) return 0;
            Control_Msg_t control_msg;
            Data_To_Control(msg->data,&control_msg);
            App_HandleControl(&control_msg);
            break;
        }
    case CAN_ID_STATUS:
        {
            if (msg->length < 6) return 0;
            Status_Msg_t status_msg;
            Data_To_Status(msg->data,&status_msg);
            break;
        }

    case CAN_ID_HEARTBEAT:
        {
            if (msg->length < 2) return 0;
            Heartbeat_Msg_t hb_msg;
            Data_To_Heartbeat(msg->data,&hb_msg);
            break;
        }

    case CAN_ID_REGISTER:
        {
            if (msg->length < 3) return 0;
            Register_Msg_t reg_msg;
            Data_To_Register(msg->data,&reg_msg);
            break;
        }

    case CAN_ID_ACK:
        {
            if (msg->length < 5) return 0;
            Ack_Msg_t ack_msg;
            Data_To_Ack(msg->data,&ack_msg);
            App_HandleAck(&ack_msg);
            break;
        }

    default:
        return 0;
    }

    return 1;
}
