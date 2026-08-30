//
// Created by 双 on 2026/8/26.
//

//发送状态信息函数需要更改

#ifndef JOINTNODE_APP_H
#define JOINTNODE_APP_H
#include "protocol.h"

/*执行注册函数*/
void App_Register(void);

/*上报心跳函数*/
void App_Heartbeat(void);


/*处理控制报文函数*/
void App_HandleControl(const Control_Msg_t *ctrl);


/*获取状态信息函数*/
void App_GetStatus(Status_Msg_t* status);

/*发送状态信息*/
void App_Status();

/*处理应答信号*/
void App_HandleAck(const Ack_Msg_t *ack);

#endif //JOINTNODE_APP_H
