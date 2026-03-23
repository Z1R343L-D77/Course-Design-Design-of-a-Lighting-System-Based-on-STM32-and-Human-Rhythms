/**
  ******************************************************************************
  * @file           : bsp_filter.h
  * @author         : 19816
  * @brief          : None
  * @attention      : None
  * @date           : 2025/12/25
  ******************************************************************************
  */

#ifndef INC_005_CALENDAR_BSP_FILTER_H
#define INC_005_CALENDAR_BSP_FILTER_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

// 低通滤波器结构体
typedef struct {
    float output;           // 滤波器输出值
    float alpha;            // 滤波系数 (0.0~1.0)
    uint32_t sample_count;  // 采样计数
    bool is_initialized;    // 初始化标志
} LowPassFilter_t;

/**
  * @brief  初始化低通滤波器
  * @param  filter: 滤波器指针
  * @param  alpha: 滤波系数 (越小滤波越强)
  * @param  initial_value: 初始值
  */
void LowPassFilter_Init(LowPassFilter_t *filter, float alpha, float initial_value);

/**
  * @brief  重置滤波器（使用新的初始值）
  * @param  filter: 滤波器指针
  * @param  initial_value: 新的初始值
  */
void LowPassFilter_Reset(LowPassFilter_t *filter, float initial_value);

/**
  * @brief  设置滤波系数
  * @param  filter: 滤波器指针
  * @param  alpha: 新的滤波系数 (0.0~1.0)
  */
void LowPassFilter_SetAlpha(LowPassFilter_t *filter, float alpha);

/**
  * @brief  处理输入数据（一阶低通滤波）
  * @param  filter: 滤波器指针
  * @param  input: 输入数据
  * @retval 滤波后的输出数据
  */
float LowPassFilter_Process(LowPassFilter_t *filter, float input);

/**
  * @brief  处理整数输入数据
  * @param  filter: 滤波器指针
  * @param  input: 整数输入数据
  * @retval 滤波后的输出数据（整数）
  */
uint32_t LowPassFilter_ProcessInt(LowPassFilter_t *filter, uint32_t input);

/**
  * @brief  批量处理数据数组
  * @param  filter: 滤波器指针
  * @param  input_array: 输入数组
  * @param  output_array: 输出数组
  * @param  length: 数组长度
  */
void LowPassFilter_ProcessArray(LowPassFilter_t *filter,
                                const float *input_array,
                                float *output_array,
                                uint32_t length);

/**
  * @brief  获取当前输出值
  * @param  filter: 滤波器指针
  * @retval 当前滤波输出值
  */
float LowPassFilter_GetOutput(LowPassFilter_t *filter);

/**
  * @brief  获取采样计数
  * @param  filter: 滤波器指针
  * @retval 已处理的采样数
  */
uint32_t LowPassFilter_GetSampleCount(LowPassFilter_t *filter);

/**
  * @brief  判断滤波器是否已稳定（根据采样次数）
  * @param  filter: 滤波器指针
  * @param  stable_samples: 稳定所需的最小采样数
  * @retval 是否已稳定
  */
bool LowPassFilter_IsStable(LowPassFilter_t *filter, uint32_t stable_samples);

/**
  * @brief  计算时间常数对应的alpha值
  * @param  time_constant: 时间常数（秒）
  * @param  sample_period: 采样周期（秒）
  * @retval 对应的alpha值
  */
float LowPassFilter_CalculateAlpha(float time_constant, float sample_period);

#ifdef __cplusplus
}
#endif


#endif //INC_005_CALENDAR_BSP_FILTER_H
