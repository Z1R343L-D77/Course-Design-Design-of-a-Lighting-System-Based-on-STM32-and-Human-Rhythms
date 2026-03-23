/**
  ******************************************************************************
  * @file           : bsp_pid.c
  * @author         : 19816
  * @brief          : None
  * @attention      : None
  * @date           : 2025/12/25
  ******************************************************************************
  */
#include "bsp_pid.h"

#include <math.h>

void PID_Init(PID_Controller_t *pid, float kp, float ki, float kd, float target)
{
    pid->kp = kp;
    pid->ki = ki;
    pid->kd = kd;
    pid->target = target;
    pid->last_error = 0;
    pid->integral = 0;
    pid->output = 0;
    pid->output_max = 100.0f;   // PWM最大100%
    pid->output_min = 0.0f;     // PWM最小0%
}

float PID_Update(PID_Controller_t *pid, float current_value, float dt)
{
    if(dt <= 0) dt = 0.001f;  // 防止除以0

    // 1. 计算当前误差
    float error = pid->target - current_value;

    // 2. 比例项
    float p_term = pid->kp * error;

    // 3. 积分项（抗积分饱和）
    pid->integral += error * dt;

    // 积分限幅
    float i_max = pid->output_max / pid->ki;
    float i_min = pid->output_min / pid->ki;

    if(pid->integral > i_max) pid->integral = i_max;
    if(pid->integral < i_min) pid->integral = i_min;

    float i_term = pid->ki * pid->integral;

    // 4. 微分项
    float derivative = (error - pid->last_error) / dt;
    float d_term = pid->kd * derivative;

    // 5. 计算输出
    float output = p_term + i_term + d_term;

    // 6. 输出限幅
    if(output > pid->output_max) output = pid->output_max;
    if(output < pid->output_min) output = pid->output_min;

    // 7. 保存状态
    pid->last_error = error;
    pid->output = output;

    return output;
}

void PID_SetTarget(PID_Controller_t *pid, float target)
{
    pid->target = target;
    pid->integral = 0;  // 重置积分（可选）
    pid->last_error = 0;
}

void PID_Reset(PID_Controller_t *pid)
{
    pid->integral = 0;
    pid->last_error = 0;
    pid->output = 0;
}
