//
// Created by 双 on 2026/8/29.
//

#include "pid.h"

PID_t pid;

/*PID初始化（赋值）函数*/
void PID_Init(PID_t *pid)
{
    pid->kp=1.5f;
    pid->ki=0.5f;
    pid->integral=0.0f;
    pid->output_max=95.0f;
    pid->output_min=0.0f;
    pid->integral_max=2000.0f;
}

/*PID计算*/
float PID_Calculate(PID_t *pid,float target_rpm,float actual_rpm)
{
    float error;
    float output;

    error=target_rpm-actual_rpm;

    pid->integral+=error;

    /*积分限幅*/
    if (pid->integral>pid->integral_max)
    {
        pid->integral=pid->integral_max;
    }
    if (pid->integral<-pid->integral_max)
    {
        pid->integral=-pid->integral_max;
    }

    output=pid->kp*error+pid->ki*pid->integral;

    /*输出限幅*/
    if (output>pid->output_max)
    {
        output=pid->output_max;
    }
    if (output<pid->output_min)
    {
        output=pid->output_min;
    }

    return output;
}