/**
  ******************************************************************************
  * @file           : myWB2812.h
  * @author         : 19816
  * @brief          : None
  * @attention      : None
  * @date           : 2025/12/21
  ******************************************************************************
  */

#ifndef INC_005_CALENDAR_MYWB2812_H
#define INC_005_CALENDAR_MYWB2812_H

#include "bsp_system.h"

#include "main.h"
#include "tim.h"

// 硬件配置
#define WS_H           119   // 1码的PWM比较值
#define WS_L           59   // 0码的PWM比较值
#define LED_NUM        4   // WS2812灯个数
#define DATA_LEN       24   // 每个灯24位数据
#define WS2812_RST_NUM 40   // 复位周期数



// 函数声明
void WS2812_Init(void);
void WS2812_Set(uint16_t num, uint8_t R, uint8_t G, uint8_t B);
void WS2812_Fill(uint8_t R, uint8_t G, uint8_t B);
void WS2812_Clear(void);
void WS2812_Update(void);

// 台灯控制函数
void WS2812_SetLamp(uint8_t brightness, uint8_t color_temp);


// DMA控制
void WS2812_StartDMA(void);
void WS2812_StopDMA(void);
uint8_t WS2812_IsBusy(void);

extern uint8_t lamp_status;
void task_lamp_proc(void);

#endif //INC_005_CALENDAR_MYWB2812_H
