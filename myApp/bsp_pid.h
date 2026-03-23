/**
  ******************************************************************************
  * @file           : bsp_pid.h
  * @author         : 19816
  * @brief          : None
  * @attention      : None
  * @date           : 2025/12/25
  ******************************************************************************
  */

#ifndef INC_005_CALENDAR_BSP_PID_H
#define INC_005_CALENDAR_BSP_PID_H


typedef struct {
    float kp;          // 比例系数
    float ki;          // 积分系数
    float kd;          // 微分系数

    float target;      // 目标值 (lux)
    float last_error;  // 上次误差
    float integral;    // 积分累计
    float output;      // 输出值 (PWM占空比 0-100)
    float output_max;  // 输出最大值
    float output_min;  // 输出最小值
} PID_Controller_t;

void PID_Init(PID_Controller_t *pid, float kp, float ki, float kd, float target);
float PID_Update(PID_Controller_t *pid, float current_value, float dt);
void PID_SetTarget(PID_Controller_t *pid, float target);
void PID_Reset(PID_Controller_t *pid);


#endif //INC_005_CALENDAR_BSP_PID_H
