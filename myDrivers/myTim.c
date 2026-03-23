/**
  ******************************************************************************
  * @file           : myTim.c
  * @author         : 19816
  * @brief          : None
  * @attention      : None
  * @date           : 2025/12/22
  ******************************************************************************
  */
#include "myTim.h"


/**
 * @brief  初始化PWM输出（TIM4_CH2, PA12）
 */
void myPWM_Init(void){
    // 启动TIM4的PWM通道2
    HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_2);

    // 初始设置占空比为50%（假设ARR=255）
    __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_2, 255);

    PWM_SetPercent(light_duty*10);
}

/**
 * @brief  设置PWM占空比（百分比）
 * @param  percent: 占空比百分比 0-100
 */
void PWM_SetPercent(uint8_t percent){
    if (percent > 100) percent = 100;

    // 根据ARR值计算占空比
    uint32_t arr = __HAL_TIM_GET_AUTORELOAD(&htim4);
    uint32_t pulse = (arr * percent) / 100;

    __HAL_TIM_SET_COMPARE(&htim4, TIM_CHANNEL_2, pulse);
}
