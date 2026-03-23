/**
  ******************************************************************************
  * @file           : myDelay.c
  * @author         : 19816
  * @brief          : None
  * @attention      : None
  * @date           : 2025/12/21
  ******************************************************************************
  */

#include "myDelay.h"

// 全局变量定义
volatile uint32_t sysTickUptime = 0;

// HAL库兼容的SysTick处理函数
void SysTick_Handler_Custom(void)
{
    sysTickUptime++;
    // 如果使用HAL库的延时函数，也需要调用HAL的SysTick处理
    HAL_IncTick();
}

///**
//  * @brief  重写HAL_GetTick函数以使用自定义的计时器
//  * @note   如果你想要完全控制tick计数，可以重写这个函数
//  */
//__weak uint32_t HAL_GetTick(void)
//{
//    return sysTickUptime;
//}

void SystickInit(void)
{
    // 使用HAL库的SysTick配置，1ms中断
    // 在main.c中HAL_Init()已经配置了SysTick

    // 或者手动配置（如果不想依赖HAL_Init）：
    // HAL_SYSTICK_Config(SystemCoreClock / 1000U);
}

uint32_t micros(void)
{
    uint32_t ms;
    uint32_t cycle_cnt;
    uint32_t ticks_per_ms = HAL_RCC_GetSysClockFreq() / 1000U;  // 每毫秒的tick数

    // 禁用中断，防止在读取过程中被中断
    uint32_t primask = __get_PRIMASK();
    __disable_irq();

    // 获取当前毫秒计数
    ms = sysTickUptime;

    // 获取当前SysTick计数值
    cycle_cnt = SysTick->VAL;

    // 恢复中断状态
    __set_PRIMASK(primask);

    // 计算微秒数
    if (SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) {
        // 如果在读取过程中发生了SysTick中断
        ms = sysTickUptime;
        cycle_cnt = SysTick->VAL;
    }

    // 计算微秒数
    // 注意：SysTick是向下计数的
    uint32_t cycles_per_us = HAL_RCC_GetSysClockFreq() / 1000000U;
    uint32_t us = ms * 1000UL + (ticks_per_ms - cycle_cnt) / cycles_per_us;

    return us;
}

uint32_t millis(void)
{
    return micros() / 1000UL;
    // 或者直接返回：return sysTickUptime;
}

void delayMicroseconds(uint32_t us)
{
    if (us == 0) return;

    uint32_t start = micros();

    // 对于较短的延时，使用循环
    if (us < 1000) {
        // 计算需要的CPU周期数
        uint32_t cycles = us * (SystemCoreClock / 1000000U);
        volatile uint32_t i;
        for (i = 0; i < cycles; i++) {
            __NOP();  // 空操作
        }
    } else {
        // 对于较长的延时，使用micros()轮询
        while ((int32_t)(micros() - start) < us) {
            // 等待
        }
    }
}

void delay(uint32_t ms)
{
    // 使用HAL库的延时函数，精度更高
    HAL_Delay(ms);
}

void delay_ms(uint32_t x)
{
    delay(x);
}

void delay_us(uint32_t x)
{
    delayMicroseconds(x);
}


void get_systime(systime *sys)
{
    uint32_t us = micros();

    if (sys != NULL) {
        sys->sec = us / 1000000UL;
        sys->ms = (us % 1000000UL) / 1000UL;
        sys->us = us % 1000UL;
    }
}

float get_systime_ms(void)
{
    return (float)micros() / 1000.0f;
}

uint32_t get_systick_ms(void)
{
    return millis();
}


// 精确延时函数（使用DWT，如果可用）
#ifdef DWT_BASE
void delay_us_precise(uint32_t us)
{
    static uint8_t dwt_initialized = 0;

    if (!dwt_initialized) {
        // 使能DWT
        CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
        DWT->CYCCNT = 0;
        DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
        dwt_initialized = 1;
    }

    uint32_t cycles = us * (SystemCoreClock / 1000000U);
    uint32_t start = DWT->CYCCNT;

    while ((DWT->CYCCNT - start) < cycles) {
        // 等待
    }
}
#endif
