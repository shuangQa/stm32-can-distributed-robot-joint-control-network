# 03\_封装CAN通信层（CAN协议、驱动）

# CAN驱动

## 简单的报文发送函数框架/思路

```C
uint8_t CAN_SendMessage(CAN_Message_t *msg)
{
    *//检查传入数据正确*
*    *if (msg == NULL || msg->data == NULL || msg->length > 8 || msg->id > 0x7FF)
        return 0;

    CAN_TxHeaderTypeDef TxHeader;   *//创建发送报文的头部信息（仲裁段：ID、控制段：数据长度、数据段）的结构体变量*

*    *memset(&TxHeader, 0, sizeof(TxHeader));   *//初始化结构体*

*    //设置报文头部信息（ID，类型，长度）*
*    *TxHeader.TransmitGlobalTime=*DISABLE*;
    TxHeader.IDE   = CAN_ID_STD;   *//标识符类型：标准真*
*    *TxHeader.RTR   = CAN_RTR_DATA;   *//标识符帧类型：数据帧*
*    *TxHeader.StdId =msg->id;   *//标准标识符*
*    *TxHeader.DLC   =msg->length;   *//数据长度（0~8字节），最开始用的sizeof*

*    //发送报文*
*    *uint32_t TxMailbox=0;
    if (HAL_CAN_AddTxMessage(&hcan, &TxHeader, msg->data, &TxMailbox)!=*HAL_OK*)
    {
        return 0;
    };
    return 1;
}
```

代码16行：最开始长度使用sizeof（data），但是data在参数表里是\*data，这样测得是指针地址数据的大小，后来直接用的length，后来参数列表改了，直接用到结构体中的msg\-\>length。

## 最终的CAN驱动程序

### can\_driver\.h

```C++
*//*
*// Created by 双 on 2026/8/26.*
*//*

#ifndef MASTER_CAN_DRIVER_H
#define MASTER_CAN_DRIVER_H

#include <stdint.h>

#include "stm32f1xx_hal.h"
#include"../Protocol/protocol.h"

*//函数接口*
uint8_t CAN_Driver_Init(void);

uint8_t CAN_SendMessage(CAN_Message_t *msg);

uint8_t CAN_ReceiveMessage(CAN_Message_t *msg);

#endif *//MASTER_CAN_DRIVER_H*
```

### can\_driver\.c

```C
*//*
*// Created by 双 on 2026/8/26.*
*//*


#include "stm32f1xx_hal.h"

#include "can_driver.h"

#include "can.h"


#include <string.h>


#include"can.h"

*/*全局变量BEGIN*/*

*/*全部变量END*/*

uint8_t CAN_Driver_Init(void)
{
    *//开始配置过滤器，对结构体内各参数赋值（当前:接收所有ID）*
*    *CAN_FilterTypeDef  sFilterConfig;   *//创建过滤器配置结构体的变量*
*    *sFilterConfig.FilterActivation = *ENABLE*;
    sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
    sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
    sFilterConfig.FilterIdHigh = 0x0000;
    sFilterConfig.FilterIdLow = 0x0000;
    sFilterConfig.FilterMaskIdHigh = 0x0000;
    sFilterConfig.FilterMaskIdLow = 0x0000;
    sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;
    sFilterConfig.FilterBank = 0;
    sFilterConfig.SlaveStartFilterBank = 0;

    *//将过滤器配置应用*
*    *if (HAL_CAN_ConfigFilter(&hcan,&sFilterConfig)!=*HAL_OK*)
    {
        return 0;
    };

    *//启动CAN外设*
*    *if (HAL_CAN_Start(&hcan)!=*HAL_OK*)
    {
        return 0;
    };

    *//使能CAN接收中断*
*    *if (HAL_CAN_ActivateNotification(&hcan,CAN_IT_RX_FIFO0_MSG_PENDING)!=*HAL_OK*)
    {
        return 0;
    };

    return 1;

}

uint8_t CAN_SendMessage(CAN_Message_t *msg)
{
    *//检查传入数据正确*
*    *if (msg == NULL || msg->data == NULL || msg->length > 8 || msg->id > 0x7FF)
        return 0;

    CAN_TxHeaderTypeDef TxHeader;   *//创建发送报文的头部信息（仲裁段：ID、控制段：数据长度、数据段）的结构体变量*

*    *memset(&TxHeader, 0, sizeof(TxHeader));   *//初始化结构体*

*    //设置报文头部信息（ID，类型，长度）*
*    *TxHeader.TransmitGlobalTime=*DISABLE*;
    TxHeader.IDE   = CAN_ID_STD;   *//标识符类型：标准真*
*    *TxHeader.RTR   = CAN_RTR_DATA;   *//标识符帧类型：数据帧*
*    *TxHeader.StdId =msg->id;   *//标准标识符*
*    *TxHeader.DLC   =msg->length;   *//数据长度（0~8字节）*

*    //发送报文*
*    *uint32_t TxMailbox=0;
    if (HAL_CAN_AddTxMessage(&hcan, &TxHeader, msg->data, &TxMailbox)!=*HAL_OK*)
    {
        return 0;
    };
    return 1;
}

uint8_t CAN_ReceiveMessage(CAN_Message_t *msg)
{
    CAN_RxHeaderTypeDef RxHeader;   *//定义保存接收到的报文头部信息的结构体的变量*

*    //读取FIFO的消息（当前：FIFO0）*
*    *if(HAL_CAN_GetRxMessage(&hcan,CAN_RX_FIFO0,&RxHeader,msg->data) != *HAL_OK*)
    {
        return 0;
    }

    msg->id = RxHeader.StdId;   *//获取ID*
*    *msg->length = RxHeader.DLC;   *//获取长度*

*    *return 1;
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{

    CAN_Message_t msg;

    if(CAN_ReceiveMessage(&msg))
    {
        Protocol_Parse(&msg);
        *//这里跳转到状态机，不要在中断里处理函数*

*    *}

}
```

# 协议层

## 遇到的问题

小灯不闪烁

打断点\+判断语句：Protocol发送函数错误

## 调试过程

### 第一步

先找配置

时间片分配没问题

注意到，时间片大小不对，是2250ns不是500ns,但BS1 BS2 SJW分配无误，

找时钟

发现Master的时钟忘记配置主频72Mhz了。

解决。

### 第二步

小灯依旧没反应，主控无错误，进入节点

能进入回调函数

然后小灯没闪烁

步入回调函数

接着走向接收数据分析函数

逐步走，发现直接返回0跳出了分析函数，发现问题

```C++

uint8_t CAN_ReceiveMessage(CAN_Message_t *msg)
{
    //检查传入数据正确
    if (msg == NULL || msg->data == NULL || msg->length > 8 || msg->id > 0x7FF)
        return 0;

    CAN_RxHeaderTypeDef RxHeader;   //定义保存接收到的报文头部信息的结构体的变量

    //读取FIFO的消息（当前：FIFO0）
    if(HAL_CAN_GetRxMessage(&hcan,CAN_RX_FIFO0,&RxHeader,msg->data) != HAL_OK)
    {
        return 0;
    }

    msg->id = RxHeader.StdId;   //获取ID
    msg->length = RxHeader.DLC;   //获取长度

    return 1;
}
```

删掉了检查传入数据是否正确的代码，在发送函数中可以使用，接收函数的参数本身就是刚刚定义的用来存储接受值的变量，本身很容易是空，且可以是空。

解决问题，小灯成功闪烁1s一次，协议层成功。

## 最终成果：初版CAN协议层

### protocol\.h

```C++
*//*
*// Created by 双 on 2026/8/26.*
*//*

#ifndef MASTER_PROTOCOL_H
#define MASTER_PROTOCOL_H

#include <stdint.h>

*/*CAN报文ID定义，按优先级*/*
#define CAN_ID_CONTROL 0x100  *//主控->节点：控制命令*
#define CAN_ID_STATUS 0x200   *//节点->主控：状态反馈*
#define CAN_ID_HEARTBEAT 0x300   *//节点->主控：心跳*
#define CAN_ID_REGISTER 0x400   *//节点->主控：注册*
#define CAN_ID_ACK 0x500   *//节点->主控：应答*

*/*节点ID定义*/*
#define NODE_ID_MASTER 0x00   *//主控节点*
#define NODE_ID_JOINT1 0x01   *//电机节点1*
#define NODE_ID_JOINT2 0x02   *//电机节点2*
#define NODE_ID_BROADCAST 0xFF   *//广播地址*

*/* 控制命令使能 */*
#define MOTOR_DISABLE           0x00
#define MOTOR_ENABLE            0x01

*/* 数据长度固定，多余位自动不处理，简化程序 */*
#define CAN_DATA_LEN            8

*/*节点类型*/*
#define NODE_TYPE_MASTER  0x0001
#define NODE_TYPE_MOTOR   0x0002
#define NODE_TYPE_SENSOR  0x0003

*/* 故障码（暂作示例，具体故障信息与故障码还需要更改，即此处与实际不符） */*
#define FAULT_NONE              0x00   *//无故障*
#define FAULT_OVERCURRENT       0x01   *//电机驱动电流超过安全阈值*
#define FAULT_OVERTEMP          0x02   *//电机或驱动器温度过高*
#define FAULT_UNDERVOLTAGE      0x03   *//电源电压低于最低工作电压*
#define FAULT_OVERVOLTAGE       0x04   *//电源电压高于最高允许值*
#define FAULT_ENCODER_ERROR     0x05   *//编码器信号异常（丢失、相位错误等）*
#define FAULT_STALL             0x06   *//电机堵转（期望转速与实际转速偏差过大且持续一段时间）*
#define FAULT_COMM_LOST         0x07   *//通信丢失（节点与主控失去联系，或内部通信故障）*


*/*通用CAN报文头部信息结构体*/*
typedef struct
{
    uint32_t id;

    uint8_t length;

    uint8_t data[8];

}CAN_Message_t;


*/* 节点状态 */*
typedef enum
{
    *NODE_STATE_NORMAL*=0x00,   *//正常运行*
*    NODE_STATE_WARNING*,       *//警告（弱异常）*
*    NODE_STATE_FAULT*,         *//故障（严重异常）*
}NodeState_t;

*/*各类型报文的数据结构体*/*
*//控制报文（0x100）*
typedef struct
{
    uint8_t  node_id;           *// 目标节点 ID*
*    *int16_t  target_rpm;        *// 目标转速*
*    *uint8_t  enable;            *// 使能*
*    *uint8_t  seq_num;           *// 命令序列号：使控制命令与应答信号匹配，确保命令被正确执行*
*    *uint8_t  reserved[3];       *// 保留*
}Control_Msg_t;

*//状态报文（0x200）*
typedef struct
{
    uint8_t  node_id;
    int16_t  actual_rpm;        *//当前转速*
*    *uint8_t  pwm;               *//占空比*
*    *uint8_t  state;             *//状态位*
*    *uint8_t  fault_code;        *//详细故障信息（故障码）*
*    *uint8_t  reserved[2];
}Status_Msg_t;

*/* 心跳报文 (0x300) */*
typedef struct {
    uint8_t  node_id;
    uint8_t  state;
    uint8_t  reserved[6];
} Heartbeat_Msg_t;

*/* 注册报文 (0x400) */*
typedef struct {
    uint8_t  node_id;
    uint16_t node_type;
    uint8_t  reserved[5];
} Register_Msg_t;

*/* ACK 报文 (0x500) */*
typedef struct {
    uint8_t  node_id;
    uint16_t acked_id;          *// 被确认的 CAN ID*
*    *uint8_t  seq_num;           *//命令序列号*
*    *uint8_t  result;            *//应答结果：成功/失败*
*    *uint8_t  reserved[3];
} Ack_Msg_t;

*/* 辅助函数：将结构体转换为 8 字节数据数组（小端） */*
void Control_To_Data(const Control_Msg_t *msg, uint8_t *data);
void Status_To_Data(const Status_Msg_t *msg, uint8_t *data);
void Heartbeat_To_Data(const Heartbeat_Msg_t *msg, uint8_t *data);
void Register_To_Data(const Register_Msg_t *msg, uint8_t *data);
void Ack_To_Data(const Ack_Msg_t *msg, uint8_t *data);

*/* 辅助函数：从 8 字节数据数组解析为结构体 */*
void Data_To_Control(const uint8_t *data, Control_Msg_t *msg);
void Data_To_Status(const uint8_t *data, Status_Msg_t *msg);
void Data_To_Heartbeat(const uint8_t *data, Heartbeat_Msg_t *msg);
void Data_To_Register(const uint8_t *data, Register_Msg_t *msg);
void Data_To_Ack(const uint8_t *data, Ack_Msg_t *msg);

*/*各报文发送函数*/*
uint8_t Protocol_SendControl(const Control_Msg_t *msg);
uint8_t Protocol_SendStatus(const Status_Msg_t *msg);
uint8_t Protocol_SendHeartbeat(const Heartbeat_Msg_t *msg);
uint8_t Protocol_SendRegister(const Register_Msg_t *msg);
uint8_t Protocol_SendAck(const Ack_Msg_t *msg);

*/*报文接收后的分析函数（未引入设备管理）*/*
uint8_t Protocol_Parse(const CAN_Message_t *msg);

#endif *//MASTER_PROTOCOL_H*
```

### protocol\.c

```C++
*//*
*// Created by 双 on 2026/8/26.*
*//*

#include "protocol.h"

#include <string.h>

#include "../CAN/can_driver.h"

*/*16位数据转为8位形式存储进报文*/*
static void PutU16(uint8_t *buf,uint16_t val)
{
    buf[0]=(uint8_t)(val&0xFF);
    buf[1]=(uint8_t)(val>>8&0xFF);
}
static void PutI16(uint8_t *buf, int16_t val) {
    PutU16(buf, (uint16_t)val);
}

*/*8位数据转为16位形式从报文中读取后存入结构体*/*
static uint16_t GetU16(const uint8_t *buf) {
    return (uint16_t)(buf[0]) | ((uint16_t)buf[1] << 8);
}

static int16_t GetI16(const uint8_t *buf) {
    return (int16_t)GetU16(buf);
}

*/* 辅助函数：将各种报文结构体转换为 8 字节数据数组 */*
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

*/* 辅助函数：从 8 字节数据数组解析为各种报文结构体 */*
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

*/*各类型报文发送*/*
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

*/*报文接收后的分析函数（未引入设备管理）*/*
uint8_t Protocol_Parse(const CAN_Message_t *msg)
{
    if(msg == NULL) return 0;
    switch (msg->id)
    {
    case CAN_ID_CONTROL:
        {
            Control_Msg_t control_msg;
            Data_To_Control(msg->data,&control_msg);
            break;
        }
    case CAN_ID_STATUS:
        {
            Status_Msg_t status_msg;
            Data_To_Status(msg->data,&status_msg);
            break;
        }

    case CAN_ID_HEARTBEAT:
        {
            Heartbeat_Msg_t hb_msg;
            Data_To_Heartbeat(msg->data,&hb_msg);
            break;
        }

    case CAN_ID_REGISTER:
        {
            Register_Msg_t reg_msg;
            Data_To_Register(msg->data,&reg_msg);
            break;
        }

    case CAN_ID_ACK:
        {
            Ack_Msg_t ack_msg;
            Data_To_Ack(msg->data,&ack_msg);
            break;
        }

    default:
        break;
    }
}
```



