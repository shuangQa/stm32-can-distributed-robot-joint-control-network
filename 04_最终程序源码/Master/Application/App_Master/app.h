//
// Created by 双 on 2026/8/26.
//

/*
 * 控制节点函数需要重写
 * 注册报文处理函数需要改应答信号的命令序列号
 */

#ifndef MASTER_APP_H
#define MASTER_APP_H
#include "protocol.h"

/*注册报文处理函数*/
void App_HandleRegister(const Register_Msg_t *msg);

/*控制节点函数*/
void App_SendControlcmd(int16_t target_rpm,uint8_t NODE_ID);


/*处理设备应答报文的函数*/
void App_HandleAck(const Ack_Msg_t *msg);

/*处理心跳报文的函数*/
void App_HandleHeartbeat(const Heartbeat_Msg_t *msg);

/*处理状态报文的函数*/
void App_HandleStatus(const Status_Msg_t *msg);

#endif //MASTER_APP_H
