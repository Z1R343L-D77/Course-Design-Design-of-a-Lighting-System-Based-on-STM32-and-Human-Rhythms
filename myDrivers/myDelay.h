/**
  ******************************************************************************
  * @file           : myDelay.h
  * @author         : 19816
  * @brief          : None
  * @attention      : None
  * @date           : 2025/12/21
  ******************************************************************************
  */

#ifndef INC_005_CALENDAR_MYDELAY_H
#define INC_005_CALENDAR_MYDELAY_H

#include "bsp_system.h"

#ifdef __cplusplus
extern "C" {
#endif

// 系统时钟配置（根据你的实际配置调整）
#ifndef CPUCLK_FREQ
#define CPUCLK_FREQ    72000000U    // 默认72MHz，请根据实际调整
#endif

// 时间结构体
typedef struct {
    uint32_t sec;      // 秒
    uint32_t ms;       // 毫秒
    uint32_t us;       // 微秒
} systime;

// 全局变量声明
extern volatile uint32_t sysTickUptime;

// 函数声明

/**
 * @brief  SysTick中断处理函数
 * @note   需要在stm32f1xx_it.c中调用或在CubeMX中配置
 */
void SysTick_Handler_Custom(void);

/**
 * @brief  初始化SysTick定时器
 * @note   1ms中断一次
 */
void SystickInit(void);

/**
 * @brief  获取当前微秒数
 * @return 自启动以来的微秒数
 */
uint32_t micros(void);

/**
 * @brief  获取当前毫秒数
 * @return 自启动以来的毫秒数
 */
uint32_t millis(void);

/**
 * @brief  微秒级延时
 * @param  us: 延时的微秒数
 */
void delayMicroseconds(uint32_t us);

/**
 * @brief  毫秒级延时（使用自定义函数）
 * @param  ms: 延时的毫秒数
 */
void delay(uint32_t ms);

/**
 * @brief  毫秒级延时（别名函数）
 * @param  x: 延时的毫秒数
 */
void delay_ms(uint32_t x);

/**
 * @brief  微秒级延时（别名函数）
 * @param  x: 延时的微秒数
 */
void delay_us(uint32_t x);


/**
 * @brief  获取系统时间结构体
 * @param  sys: 时间结构体指针
 */
void get_systime(systime *sys);

/**
 * @brief  获取系统时间（毫秒，浮点数）
 * @return 自启动以来的毫秒数（浮点）
 */
float get_systime_ms(void);

/**
 * @brief  获取系统时间（毫秒，整数）
 * @return 自启动以来的毫秒数（整数）
 */
uint32_t get_systick_ms(void);

#ifdef __cplusplus
}
#endif


#endif //INC_005_CALENDAR_MYDELAY_H
