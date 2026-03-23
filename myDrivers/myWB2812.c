/**
  ******************************************************************************
  * @file           : myWB2812.c
  * @author         : 19816
  * @brief          : None
  * @attention      : None
  * @date           : 2025/12/21
  ******************************************************************************
  */
#include "myWB2812.h"

uint8_t lamp_status = 0;
// 显存数组（全局变量）
uint16_t WS2812_RGB_Buff[LED_NUM * DATA_LEN + WS2812_RST_NUM] = {0};

// DMA传输完成标志
static volatile uint8_t dma_complete = 1;

/**
 * @brief  WS2812单灯设置函数
 * @param  num: 灯的位置 (0 ~ LED_NUM-1)
 * @param  R: 红色亮度 (0-255)
 * @param  G: 绿色亮度 (0-255)
 * @param  B: 蓝色亮度 (0-255)
 * @note   注意：WS2812是GRB顺序
 */
void WS2812_Set(uint16_t num, uint8_t R, uint8_t G, uint8_t B){
    if (num >= LED_NUM) return;

    uint32_t index = num * DATA_LEN;  // 每个灯24位数据

    // 填充绿色数据 (8位)
    for (uint8_t i = 0; i < 8; i++)
    {
        // 注意：这里是发送最高位优先
        WS2812_RGB_Buff[index + i] = ((G << i) & 0x80) ? WS_H : WS_L;
    }

    // 填充红色数据 (8位)
    for (uint8_t i = 0; i < 8; i++)
    {
        WS2812_RGB_Buff[index + 8 + i] = ((R << i) & 0x80) ? WS_H : WS_L;
    }

    // 填充蓝色数据 (8位)
    for (uint8_t i = 0; i < 8; i++)
    {
        WS2812_RGB_Buff[index + 16 + i] = ((B << i) & 0x80) ? WS_H : WS_L;
    }
}
/**
 * @brief  填充所有LED为同一颜色
 */
void WS2812_Fill(uint8_t R, uint8_t G, uint8_t B){
    for (uint16_t i = 0; i < LED_NUM; i++)
    {
        WS2812_Set(i, R, G, B);
    }
}
/**
 * @brief  关闭所有LED
 */
void WS2812_Clear(void){
    WS2812_Fill(0, 0, 0);
}

/**
 * @brief  启动DMA传输（更新LED显示）
 */
void WS2812_Update(void){
    // 确保DMA完成
    while (WS2812_IsBusy()) {
        // 等待
    }
    // 等待至少50us的复位时间（添加在缓冲区末尾的低电平）
    uint32_t buffer_size = LED_NUM * DATA_LEN;
    for (uint32_t i = buffer_size; i < buffer_size + WS2812_RST_NUM; i++) {
        WS2812_RGB_Buff[i] = 0;  // 低电平复位信号
    }
    // 启动DMA传输
    WS2812_StartDMA();
}

/**
 * @brief  WS2812初始化函数
 */
void WS2812_Init(void){
    // 首先停止DMA和定时器
    WS2812_StopDMA();

    // 初始化显存数组的复位部分为0
    uint32_t buffer_size = LED_NUM * DATA_LEN;
    for (uint16_t i = buffer_size; i < buffer_size + WS2812_RST_NUM; i++){
        WS2812_RGB_Buff[i] = 0;  // 复位期间为低电平
    }

    // 设置所有灯为关闭状态
    WS2812_Clear();
    // 重置DMA传输完成标志
    dma_complete = 1;
    // 第一次更新
    WS2812_Update();
}

/**
 * @brief  启动DMA传输
 */
void WS2812_StartDMA(void){
    dma_complete = 0;

    // 必须先停止之前的传输
    HAL_TIM_PWM_Stop_DMA(&htim1, TIM_CHANNEL_4);

    // 重新配置DMA
    // 重要：需要手动重新初始化DMA流
    if (htim1.hdma[TIM_DMA_ID_CC4]->State == HAL_DMA_STATE_BUSY) {
        htim1.hdma[TIM_DMA_ID_CC4]->State = HAL_DMA_STATE_READY;
    }

    // 启动DMA传输
    // 注意：DMA传输的数据是PWM占空比值
    HAL_StatusTypeDef status = HAL_TIM_PWM_Start_DMA(
            &htim1,
            TIM_CHANNEL_4,
            (uint32_t *)WS2812_RGB_Buff,
            LED_NUM * DATA_LEN + WS2812_RST_NUM
    );
    if (status == HAL_OK) {
        dma_complete = 1;//完成标准
    }
}

/**
 * @brief  停止DMA传输
 */
void WS2812_StopDMA(void){
    HAL_TIM_PWM_Stop_DMA(&htim1, TIM_CHANNEL_4);
    dma_complete = 1;
}

/**
 * @brief  检查DMA是否忙
 */
uint8_t WS2812_IsBusy(void){
    return !dma_complete;
}


void task_lamp_proc(void)
{
    uint8_t hour = display_time.Hours;
    uint8_t minute = display_time.Minutes;
    uint16_t total_min = hour * 60 + minute;
    // 2. 判断时间段
    if ((total_min >= 9*60) && (total_min < 11*60)) {
        // 9:00-11:00 冷白
        WS2812_Fill(255, 180, 100);  // GRB顺序
    }
    else if ((total_min >= 17*60) && (total_min < 22*60)) {
        // 17:00-19:00 暖白
        WS2812_Fill(160, 110, 195);  // GRB顺序
    }
    else {
        // 其他时间用中间值
        WS2812_Fill(255, 255, 255);  // GRB顺序
    }
    WS2812_Update();
}


