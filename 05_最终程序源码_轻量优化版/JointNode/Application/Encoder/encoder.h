//
// Created by 双 on 2026/8/29.
//

#ifndef JOINTNODE_ENCODER_H
#define JOINTNODE_ENCODER_H
#include <stdint.h>

/*初始化或启动编码器*/
void Encoder_Init(void);

/*更新编码器计数器值和增量*/
void Encoder_Update(void);

/*返回计数器值*/
int16_t Encoder_GetCount(void);

/*返回增量值*/
int16_t Encoder_GetDelta(void);

/*返回转速*/
float Encoder_GetRpm(void);

#endif //JOINTNODE_ENCODER_H
