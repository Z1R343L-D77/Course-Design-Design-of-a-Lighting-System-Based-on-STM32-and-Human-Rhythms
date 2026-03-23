/**
  ******************************************************************************
  * @file           : myBH1750.h
  * @author         : 19816
  * @brief          : None
  * @attention      : None
  * @date           : 2025/12/21
  ******************************************************************************
  */

#ifndef INC_005_CALENDAR_MYBH1750_H
#define INC_005_CALENDAR_MYBH1750_H

#include "bsp_system.h"

// BH1750引脚宏定义（HAL库）
#define BH1750_SCL_GPIO_PORT        GPIOA
#define BH1750_SDA_GPIO_PORT        GPIOA
#define BH1750_SCL_GPIO_PIN         GPIO_PIN_1
#define BH1750_SDA_GPIO_PIN         GPIO_PIN_2

// GPIO操作宏（HAL库）
#define IIC_SCL_HIGH()              HAL_GPIO_WritePin(BH1750_SCL_GPIO_PORT, BH1750_SCL_GPIO_PIN, GPIO_PIN_SET)
#define IIC_SCL_LOW()               HAL_GPIO_WritePin(BH1750_SCL_GPIO_PORT, BH1750_SCL_GPIO_PIN, GPIO_PIN_RESET)
#define IIC_SDA_HIGH()              HAL_GPIO_WritePin(BH1750_SDA_GPIO_PORT, BH1750_SDA_GPIO_PIN, GPIO_PIN_SET)
#define IIC_SDA_LOW()               HAL_GPIO_WritePin(BH1750_SDA_GPIO_PORT, BH1750_SDA_GPIO_PIN, GPIO_PIN_RESET)
#define IIC_SDA_READ()              HAL_GPIO_ReadPin(BH1750_SDA_GPIO_PORT, BH1750_SDA_GPIO_PIN)

// 类型定义
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint8_t uchar;

#define ADDR 0x23
#define BHAddWrite     0x46      //从机地址+最后写方向位
#define BHAddRead      0x47      //从机地址+最后读方向位
#define BHPowDown      0x00      //关闭模块
#define BHPowOn        0x01      //打开模块等待测量指令
#define BHReset        0x07      //重置数据寄存器值在PowerOn模式下有效
#define BHModeH1       0x10      //高分辨率 单位1lx 测量时间120ms
#define BHModeH2       0x11      //高分辨率模式2 单位0.5lx 测量时间120ms
#define BHModeL        0x13      //低分辨率 单位4lx 测量时间16ms
#define BHSigModeH     0x20      //一次高分辨率 测量 测量后模块转到 PowerDown模式
#define BHSigModeH2    0x21      //同上类似
#define BHSigModeL     0x23      // 上类似

// 函数声明
void Single_Write_BH1750(uchar REG_Address);
void BH1750_Init(void);
void bh_data_send(u8 command);
u16 bh_data_read(void);

// IIC所有操作函数
void BH1750_IIC_Init(void);                //初始化IIC的IO口
void BH1750_IIC_Start(void);               //发送IIC开始信号
void BH1750_IIC_Stop(void);                //发送IIC停止信号
void BH1750_IIC_Send_Byte(u8 txd);         //IIC发送一个字节
u8 BH1750_IIC_Read_Byte(u8 ack);           //IIC读取一个字节
u8 BH1750_IIC_Wait_Ack(void);              //IIC等待ACK信号
void BH1750_IIC_Ack(void);                 //IIC发送ACK信号
void BH1750_IIC_NAck(void);                //IIC不发送ACK信号

void BH1750_IIC_Write_One_Byte(u8 daddr, u8 addr, u8 data);
u8 BH1750_IIC_Read_One_Byte(u8 daddr, u8 addr);

extern u16 light_level;
extern uint8_t light_duty;
extern uint32_t target_lux;
extern PID_Controller_t light_pid;
void task_bh1750_proc(void);

#endif //INC_005_CALENDAR_MYBH1750_H
