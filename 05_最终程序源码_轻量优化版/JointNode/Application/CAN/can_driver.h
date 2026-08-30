//
// Created by 双 on 2026/8/26.
//

#ifndef MASTER_CAN_DRIVER_H
#define MASTER_CAN_DRIVER_H

#include <stdint.h>

#include "stm32f1xx_hal.h"
#include"../Protocol/protocol.h"

//函数接口
uint8_t CAN_Driver_Init(void);

uint8_t CAN_SendMessage(CAN_Message_t *msg);

uint8_t CAN_ReceiveMessage(CAN_Message_t *msg);

#endif //MASTER_CAN_DRIVER_H
