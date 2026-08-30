//
// Created by 双 on 2026/8/29.
//

#ifndef JOINTNODE_PID_H
#define JOINTNODE_PID_H

/*PID结构体*/
typedef struct
{
    float kp;           //比例系数
    float ki;           //积分系数

    float integral;     //积分项

    float output_max;   //输出最大值
    float output_min;   //输出最小值

    float integral_max; //积分限幅
}PID_t;

extern PID_t pid;

/*PID初始化（赋值）函数*/
void PID_Init(PID_t *pid);

void PID_Reset(PID_t *pid);

/*PID计算*/
float PID_Calculate(PID_t *pid,float target_rpm,float actual_rpm);


#endif //JOINTNODE_PID_H
