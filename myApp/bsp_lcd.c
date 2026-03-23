/**
  ******************************************************************************
  * @file           : bsp_lcd.c
  * @author         : 19816
  * @brief          : None
  * @attention      : None
  * @date           : 2025/1/20
  ******************************************************************************
  */#include "bsp_lcd.h"
/**
 * @brief  格式化字符串并显示在指定的LCD行上。
 *
 * 该函数接受一个行号和一个格式化字符串（类似于printf），
 * 格式化字符串后，将其显示在LCD的指定行上。
 *
 * @param  Line    要显示字符串的LCD行号。
 * @param  format  格式化字符串，后跟要格式化的参数。
 *
 * 该函数内部使用 `vsprintf` 来格式化字符串，然后
 * 调用 `LCD_DisplayStringLine` 在LCD上显示格式化后的字符串。
 *
 * 示例用法:
 * @code
 * LcdSprintf(0, "Temperature: %d C", temperature);
 * @endcode
 */
void LcdSprintf(uint8_t Line, char *format,...)
{
    char String[21];  // 缓冲区用于存储格式化后的字符串
    va_list arg;      // 参数列表用于存储可变参数
    va_start(arg, format);  // 使用格式化字符串初始化参数列表
    vsprintf(String, format, arg);  // 格式化字符串并存储在缓冲区中
    va_end(arg);  // 清理参数列表
    LCD_DisplayStringLine(Line*24,String);  // 在LCD的指定行显示格式化后的字符串
}

void task_lcd_proc(void)
{
    LcdSprintf(0,">systick: %1.0d  ",sysTickUptime/1000);
    LcdSprintf(1,">time: %02u-%02u-%02u  ",display_time.Hours,display_time.Minutes,display_time.Seconds);
    LcdSprintf(2,">now_light:  %1.0d    ",light_level);
    LcdSprintf(3,">tar_light:  %1.0d    ",target_lux);
    LcdSprintf(4,">percent: %1.0d    ",light_duty);
}

void mylcd_init(void)
{
    LCD_Init();
    LCD_Clear(Black);
    LCD_SetTextColor(White);
    LCD_SetBackColor(Black);
}
