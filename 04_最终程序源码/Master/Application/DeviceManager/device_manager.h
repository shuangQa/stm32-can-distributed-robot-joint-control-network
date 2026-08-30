//
// Created by 双 on 2026/8/26.
//

#ifndef MASTER_DEVICE_MANAGER_H
#define MASTER_DEVICE_MANAGER_H

#include "protocol.h"

#define DEVICE_OFFLINE_TIMEOUT 3000

typedef struct
{
    uint8_t node_id;
    uint16_t node_type;
    uint8_t online;
    uint8_t state;
    int16_t actual_rpm;
    uint8_t pwm;
    uint8_t  fault_code;
    uint32_t last_heartbeat;
}Device_t;

//先不做可自动扩展设备表，且因此这个宏定义先挨着放
#define MAX_DEVICE_NUM 10
extern Device_t g_device_table[MAX_DEVICE_NUM];

/*根据设备ID获取在注册表中的位置*/
Device_t* DeviceManager_FindDevice(uint8_t node_id);

/*将注册报文填入设备表的靠前的空余位*/
void DeviceManager_Register(const Register_Msg_t *msg);

/*心跳信息更新到设备表的函数*/
void DeviceManager_UpdateHeartbeat(uint8_t node_id);

/*离线检测*/
void DeviceManager_CheckOffline();

/*更新状态信息函数*/
void DeviceManager_UpdateState(const Status_Msg_t* state);

#endif //MASTER_DEVICE_MANAGER_H
