//
// Created by 双 on 2026/8/26.
//

#ifndef MASTER_PROTOCOL_H
#define MASTER_PROTOCOL_H

#include <stdint.h>

/*CAN报文ID定义，按优先级*/
#define CAN_ID_CONTROL 0x100  //主控->节点：控制命令
#define CAN_ID_STATUS 0x200   //节点->主控：状态反馈
#define CAN_ID_HEARTBEAT 0x300   //节点->主控：心跳
#define CAN_ID_REGISTER 0x400   //节点->主控：注册
#define CAN_ID_ACK 0x500   //节点->主控：应答


/* 控制命令使能 */
#define MOTOR_DISABLE           0x00
#define MOTOR_ENABLE            0x01

/* 数据长度固定，多余位自动不处理，简化程序 */
#define CAN_DATA_LEN            8

/* 故障码（暂作示例，具体故障信息与故障码还需要更改，即此处与实际不符） */
#define FAULT_NONE              0x00   //无故障
#define FAULT_OVERCURRENT       0x01   //电机驱动电流超过安全阈值
#define FAULT_OVERTEMP          0x02   //电机或驱动器温度过高
#define FAULT_UNDERVOLTAGE      0x03   //电源电压低于最低工作电压
#define FAULT_OVERVOLTAGE       0x04   //电源电压高于最高允许值
#define FAULT_ENCODER_ERROR     0x05   //编码器信号异常（丢失、相位错误等）
#define FAULT_STALL             0x06   //电机堵转（期望转速与实际转速偏差过大且持续一段时间）
#define FAULT_COMM_LOST         0x07   //通信丢失（节点与主控失去联系，或内部通信故障）

/* 节点状态 */
typedef enum
{
    NODE_STATE_NORMAL=0x00,   //正常运行
    NODE_STATE_WARNING,       //警告（弱异常）
    NODE_STATE_FAULT,         //故障（严重异常）
}MasterState_t;

/*节点类型*/
typedef enum
{
    NODE_TYPE_MASTER=0x00,
    NODE_TYPE_MOTOR=0x01,
    NODE_TYPE_SENSOR=0x02,   //传感器节点，暂时还没有
}Node_Type_t;

/*节点ID定义*/
typedef enum
{
    NODE_ID_MASTER=0x00,        //主控节点
    NODE_ID_JOINT1=0x01,        //电机节点1
    NODE_ID_JOINT2=0x02,        //电机节点2
    NODE_ID_BROADCAST=0xFF,     //广播地址
}NODE_ID;

/*电机节点ID*/
//当前设备少，先不用
// typedef enum
// {
//     NODE_JOINTID_JOINT1=0x00,
//     NODE_JOINTID_JOINT2=0x01,
// }Node_JointID_t;

/*通用CAN报文头部信息结构体*/
typedef struct
{
    uint32_t id;

    uint8_t length;

    uint8_t data[8];

}CAN_Message_t;

/*各类型报文的数据结构体*/
//控制报文（0x100）
typedef struct
{
    uint8_t  node_id;           // 目标节点 ID
    int16_t  target_rpm;        // 目标转速
    uint8_t  enable;            // 使能
    uint8_t  seq_num;           // 命令序列号：使控制命令与应答信号匹配，确保命令被正确执行
    uint8_t  reserved[3];       // 保留
}Control_Msg_t;

//状态报文（0x200）
typedef struct
{
    uint8_t  node_id;
    int16_t  actual_rpm;        //当前转速
    uint8_t  pwm;               //占空比
    uint8_t  state;             //状态位
    uint8_t  fault_code;        //详细故障信息（故障码）
    uint8_t  reserved[2];
}Status_Msg_t;

/* 心跳报文 (0x300) */
typedef struct {
    uint8_t  node_id;
    uint8_t  state;
    uint8_t  reserved[6];
} Heartbeat_Msg_t;

/* 注册报文 (0x400) */
typedef struct {
    uint8_t  node_id;
    uint16_t node_type;
    uint8_t  reserved[5];
} Register_Msg_t;

/* ACK 报文 (0x500) */
typedef struct {
    uint8_t  node_id;
    uint16_t acked_id;          // 被确认的 CAN ID
    uint8_t  seq_num;           //命令序列号
    uint8_t  result;            //应答结果：成功/失败
    uint8_t  reserved[3];
} Ack_Msg_t;

/* 辅助函数：将结构体转换为 8 字节数据数组（小端） */
void Control_To_Data(const Control_Msg_t *msg, uint8_t *data);
void Status_To_Data(const Status_Msg_t *msg, uint8_t *data);
void Heartbeat_To_Data(const Heartbeat_Msg_t *msg, uint8_t *data);
void Register_To_Data(const Register_Msg_t *msg, uint8_t *data);
void Ack_To_Data(const Ack_Msg_t *msg, uint8_t *data);

/* 辅助函数：从 8 字节数据数组解析为结构体 */
void Data_To_Control(const uint8_t *data, Control_Msg_t *msg);
void Data_To_Status(const uint8_t *data, Status_Msg_t *msg);
void Data_To_Heartbeat(const uint8_t *data, Heartbeat_Msg_t *msg);
void Data_To_Register(const uint8_t *data, Register_Msg_t *msg);
void Data_To_Ack(const uint8_t *data, Ack_Msg_t *msg);

/*各报文发送函数*/
uint8_t Protocol_SendControl(const Control_Msg_t *msg);
uint8_t Protocol_SendStatus(const Status_Msg_t *msg);
uint8_t Protocol_SendHeartbeat(const Heartbeat_Msg_t *msg);
uint8_t Protocol_SendRegister(const Register_Msg_t *msg);
uint8_t Protocol_SendAck(const Ack_Msg_t *msg);

/*报文接收后的分析函数（未引入设备管理）*/
uint8_t Protocol_Parse(const CAN_Message_t *msg);

#endif //MASTER_PROTOCOL_H
