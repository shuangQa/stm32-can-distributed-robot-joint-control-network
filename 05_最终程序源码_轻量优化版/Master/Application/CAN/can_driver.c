//
// Created by 双 on 2026/8/26.
//


#include "stm32f1xx_hal.h"

#include "can_driver.h"

#include "can.h"


#include <string.h>


#include"can.h"

/*全局变量BEGIN*/

/*全部变量END*/

uint8_t CAN_Driver_Init(void)
{
    //开始配置过滤器，对结构体内各参数赋值（当前:接收所有ID）
    CAN_FilterTypeDef  sFilterConfig;   //创建过滤器配置结构体的变量
    sFilterConfig.FilterActivation = ENABLE;
    sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
    sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
    sFilterConfig.FilterIdHigh = 0x0000;
    sFilterConfig.FilterIdLow = 0x0000;
    sFilterConfig.FilterMaskIdHigh = 0x0000;
    sFilterConfig.FilterMaskIdLow = 0x0000;
    sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;
    sFilterConfig.FilterBank = 0;
    sFilterConfig.SlaveStartFilterBank = 0;

    //将过滤器配置应用
    if (HAL_CAN_ConfigFilter(&hcan,&sFilterConfig)!=HAL_OK)
    {
        return 0;
    };

    //启动CAN外设
    if (HAL_CAN_Start(&hcan)!=HAL_OK)
    {
        return 0;
    };

    //使能CAN接收中断
    if (HAL_CAN_ActivateNotification(&hcan,CAN_IT_RX_FIFO0_MSG_PENDING)!=HAL_OK)
    {
        return 0;
    };

    return 1;

}

uint8_t CAN_SendMessage(CAN_Message_t *msg)
{
    //检查传入数据正确
    if (msg == NULL || msg->length > CAN_DATA_LEN || msg->id > 0x7FF)
        return 0;

    CAN_TxHeaderTypeDef TxHeader;   //创建发送报文的头部信息（仲裁段：ID、控制段：数据长度、数据段）的结构体变量

    memset(&TxHeader, 0, sizeof(TxHeader));   //初始化结构体

    //设置报文头部信息（ID，类型，长度）
    TxHeader.TransmitGlobalTime=DISABLE;
    TxHeader.IDE   = CAN_ID_STD;   //标识符类型：标准真
    TxHeader.RTR   = CAN_RTR_DATA;   //标识符帧类型：数据帧
    TxHeader.StdId =msg->id;   //标准标识符
    TxHeader.DLC   =msg->length;   //数据长度（0~8字节）

    //发送报文
    uint32_t TxMailbox=0;
    if (HAL_CAN_AddTxMessage(&hcan, &TxHeader, msg->data, &TxMailbox)!=HAL_OK)
    {
        return 0;
    };
    return 1;
}

uint8_t CAN_ReceiveMessage(CAN_Message_t *msg)
{
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

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
    CAN_Message_t msg;

    if(CAN_ReceiveMessage(&msg))
    {
        Protocol_Parse(&msg);
        //这里跳转到状态机，不要在中断里处理函数
    }

}
