/**
  ******************************************************************************
  * @file           : bsp_filter.c
  * @author         : 19816
  * @brief          : None
  * @attention      : None
  * @date           : 2025/12/25
  ******************************************************************************
  */
#include "bsp_filter.h"
#include <math.h>
#include "stdio.h"
// 默认滤波系数
#define DEFAULT_ALPHA 0.2f

/**
  * @brief  初始化低通滤波器
  */
void LowPassFilter_Init(LowPassFilter_t *filter, float alpha, float initial_value)
{
    if(filter == NULL) return;

    // 参数有效性检查
    if(alpha <= 0.0f) alpha = DEFAULT_ALPHA;
    if(alpha > 1.0f) alpha = 1.0f;

    filter->alpha = alpha;
    filter->output = initial_value;
    filter->sample_count = 0;
    filter->is_initialized = true;
}

/**
  * @brief  重置滤波器
  */
void LowPassFilter_Reset(LowPassFilter_t *filter, float initial_value)
{
    if(filter == NULL || !filter->is_initialized) return;

    filter->output = initial_value;
    filter->sample_count = 0;
}

/**
  * @brief  设置滤波系数
  */
void LowPassFilter_SetAlpha(LowPassFilter_t *filter, float alpha)
{
    if(filter == NULL || !filter->is_initialized) return;

    // 参数有效性检查
    if(alpha <= 0.0f) alpha = DEFAULT_ALPHA;
    if(alpha > 1.0f) alpha = 1.0f;

    filter->alpha = alpha;
}

/**
  * @brief  处理输入数据（一阶低通滤波）
  * 公式：y(n) = α * x(n) + (1 - α) * y(n-1)
  */
float LowPassFilter_Process(LowPassFilter_t *filter, float input)
{
    if(filter == NULL || !filter->is_initialized) {
        return input;
    }

    // 应用一阶低通滤波公式
    filter->output = filter->alpha * input + (1.0f - filter->alpha) * filter->output;

    filter->sample_count++;

    return filter->output;
}

/**
  * @brief  处理整数输入数据
  */
uint32_t LowPassFilter_ProcessInt(LowPassFilter_t *filter, uint32_t input)
{
    if(filter == NULL || !filter->is_initialized) {
        return input;
    }

    // 转换为浮点数处理
    float input_f = (float)input;
    float output_f = LowPassFilter_Process(filter, input_f);

    // 转换回整数
    return (uint32_t)(output_f + 0.5f); // 四舍五入
}

/**
  * @brief  批量处理数据数组
  */
void LowPassFilter_ProcessArray(LowPassFilter_t *filter,
                                const float *input_array,
                                float *output_array,
                                uint32_t length)
{
    if(filter == NULL || !filter->is_initialized ||
       input_array == NULL || output_array == NULL) {
        return;
    }

    for(uint32_t i = 0; i < length; i++) {
        output_array[i] = LowPassFilter_Process(filter, input_array[i]);
    }
}

/**
  * @brief  获取当前输出值
  */
float LowPassFilter_GetOutput(LowPassFilter_t *filter)
{
    if(filter == NULL || !filter->is_initialized) {
        return 0.0f;
    }

    return filter->output;
}

/**
  * @brief  获取采样计数
  */
uint32_t LowPassFilter_GetSampleCount(LowPassFilter_t *filter)
{
    if(filter == NULL || !filter->is_initialized) {
        return 0;
    }

    return filter->sample_count;
}

/**
  * @brief  判断滤波器是否已稳定
  */
bool LowPassFilter_IsStable(LowPassFilter_t *filter, uint32_t stable_samples)
{
    if(filter == NULL || !filter->is_initialized) {
        return false;
    }

    return (filter->sample_count >= stable_samples);
}

/**
  * @brief  计算时间常数对应的alpha值
  * 公式：α = ΔT / (τ + ΔT)
  * 其中：τ为时间常数，ΔT为采样周期
  */
float LowPassFilter_CalculateAlpha(float time_constant, float sample_period)
{
    if(time_constant <= 0.0f || sample_period <= 0.0f) {
        return DEFAULT_ALPHA;
    }

    float alpha = sample_period / (time_constant + sample_period);

    // 限制在合理范围内
    if(alpha < 0.01f) alpha = 0.01f;   // 最小alpha
    if(alpha > 0.9f) alpha = 0.9f;     // 最大alpha

    return alpha;
}
