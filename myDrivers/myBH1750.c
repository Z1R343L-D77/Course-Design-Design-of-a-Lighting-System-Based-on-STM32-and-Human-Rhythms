/**
  ******************************************************************************
  * @file           : bsp_bh1750.c
  * @author         : 19816
  * @brief          : None
  * @attention      : None
  * @date           : 2025/12/21
  ******************************************************************************
  */
#include "myBH1750.h"

u16 light_level = 0;
uint8_t light_duty = 10;
// 全局滤波器实例
LowPassFilter_t lux_filter;

//PID
// 全局变量
PID_Controller_t light_pid;
uint32_t target_lux = 5000;  // 目标照度500 lux

/**
**  设置SDA为输出（开漏模式）
**/
void SDA_OUT(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Pin = BH1750_SDA_GPIO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;      // 开漏输出
    GPIO_InitStruct.Pull = GPIO_NOPULL;              // 开漏模式不需要上拉
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;  // 中等速度
    HAL_GPIO_Init(BH1750_SDA_GPIO_PORT, &GPIO_InitStruct);
}

/**
**  设置SDA为输入（带上拉）
**/
void SDA_IN(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    GPIO_InitStruct.Pin = BH1750_SDA_GPIO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;          // 输入模式
    GPIO_InitStruct.Pull = GPIO_PULLUP;              // 上拉输入
    HAL_GPIO_Init(BH1750_SDA_GPIO_PORT, &GPIO_InitStruct);
}

void Single_Write_BH1750(uchar REG_Address)
{
    BH1750_IIC_Start();                  // 起始信号
    BH1750_IIC_Send_Byte(BHAddWrite);    // 发送设备地址+写信号
    BH1750_IIC_Send_Byte(REG_Address);   // 内部寄存器地址
    BH1750_IIC_Stop();                   // 发送停止信号
}

void BH1750_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // 使能GPIOA时钟
    __HAL_RCC_GPIOA_CLK_ENABLE();

    // 初始化SCL为推挽输出
    GPIO_InitStruct.Pin = BH1750_SCL_GPIO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;      // 推挽输出
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;  // 中等速度
    HAL_GPIO_Init(BH1750_SCL_GPIO_PORT, &GPIO_InitStruct);

    // 初始化SDA为开漏输出（初始状态）
    GPIO_InitStruct.Pin = BH1750_SDA_GPIO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;      // 开漏输出
    HAL_GPIO_Init(BH1750_SDA_GPIO_PORT, &GPIO_InitStruct);

    // 初始状态设为高电平
    IIC_SCL_HIGH();
    IIC_SDA_HIGH();

    // 上电、复位、设置测量模式
    bh_data_send(BHPowOn);   // BH1750上电
    bh_data_send(BHReset);   // BH1750复位
    bh_data_send(BHModeH2);  // 设置测量模式：分辨率0.5lx，测量时间120ms

    // 初始化低通滤波器
    // 参数说明：alpha=0.2，初始值=500
    // alpha越小，滤波越强，响应越慢
    LowPassFilter_Init(&lux_filter, 0.2f, 500.0f);

    //PID_INIT
    // 初始化PID控制器
    // 参数需要调试：kp=0.5, ki=0.1, kd=0.01 是常见起始值
    PID_Init(&light_pid, 0.001f, 0.0001f, 0.0005f, target_lux);
}

void BH1750_IIC_Init(void)
{
    // 已经在BH1750_Init中初始化，这里可以留空或调用BH1750_Init
    BH1750_Init();
}

// 产生IIC起始信号
void BH1750_IIC_Start(void)
{
    SDA_OUT();               // SDA线设置为输出
    IIC_SDA_HIGH();          // SDA = 1
    IIC_SCL_HIGH();          // SCL = 1
    delay_us(4);         // 延时4us
    IIC_SDA_LOW();           // START: CLK为高时，DATA从高变低
    delay_us(4);
    IIC_SCL_LOW();           // 钳住I2C总线，准备发送或接收数据
}

// 产生IIC停止信号
void BH1750_IIC_Stop(void)
{
    SDA_OUT();               // SDA线设置为输出
    IIC_SCL_LOW();
    IIC_SDA_LOW();           // 准备停止条件
    delay_us(4);
    IIC_SCL_HIGH();          // 时钟拉高
    IIC_SDA_HIGH();          // STOP: CLK为高时，DATA从低变高
    delay_us(4);         // 停止条件完成
}

// 等待应答信号到来
// 返回值：1，接收应答失败；0，接收应答成功
u8 BH1750_IIC_Wait_Ack(void)
{
    u8 ucErrTime = 0;

    SDA_IN();                // SDA设置为输入
    IIC_SDA_HIGH();
    delay_us(1);
    IIC_SCL_HIGH();
    delay_us(1);

    // 等待SDA变低（从机应答）
    while(IIC_SDA_READ())
    {
        ucErrTime++;
        if(ucErrTime > 250)
        {
            BH1750_IIC_Stop();
            return 1;        // 超时，返回失败
        }
        delay_us(1);
    }

    IIC_SCL_LOW();           // 时钟输出0
    return 0;                // 成功接收应答
}

// 产生ACK应答
void BH1750_IIC_Ack(void)
{
    IIC_SCL_LOW();
    SDA_OUT();
    IIC_SDA_LOW();           // SDA = 0，表示ACK
    delay_us(2);
    IIC_SCL_HIGH();          // 时钟脉冲
    delay_us(2);
    IIC_SCL_LOW();
}

// 不产生ACK应答
void BH1750_IIC_NAck(void)
{
    IIC_SCL_LOW();
    SDA_OUT();
    IIC_SDA_HIGH();          // SDA = 1，表示NACK
    delay_us(2);
    IIC_SCL_HIGH();          // 时钟脉冲
    delay_us(2);
    IIC_SCL_LOW();
}

// IIC发送一个字节
void BH1750_IIC_Send_Byte(u8 txd)
{
    u8 t;
    SDA_OUT();               // 设置SDA为输出
    IIC_SCL_LOW();           // 拉低时钟开始数据传输

    for(t = 0; t < 8; t++)
    {
        // 发送最高位
        if((txd & 0x80) >> 7)
            IIC_SDA_HIGH();
        else
            IIC_SDA_LOW();

        txd <<= 1;           // 左移准备发送下一位

        delay_us(2);     // 延时
        IIC_SCL_HIGH();      // 时钟拉高
        delay_us(2);
        IIC_SCL_LOW();       // 时钟拉低
        delay_us(2);
    }
}

// 读1个字节，ack=1时，发送ACK；ack=0，发送nACK
u8 BH1750_IIC_Read_Byte(u8 ack)
{
    u8 i, receive = 0;

    SDA_IN();                // SDA设置为输入

    for(i = 0; i < 8; i++)
    {
        IIC_SCL_LOW();
        delay_us(2);
        IIC_SCL_HIGH();      // 时钟拉高
        receive <<= 1;       // 左移一位

        if(IIC_SDA_READ())   // 读取SDA线状态
            receive++;       // 如果为1，最低位置1

        delay_us(1);
    }

    if (!ack)
        BH1750_IIC_NAck();   // 发送NACK
    else
        BH1750_IIC_Ack();    // 发送ACK

    return receive;
}

void bh_data_send(u8 command)
{
    do {
        BH1750_IIC_Start();              // IIC起始信号
        BH1750_IIC_Send_Byte(BHAddWrite); // 发送器件地址+写标志
    } while(BH1750_IIC_Wait_Ack());      // 等待从机应答，失败则重试

    BH1750_IIC_Send_Byte(command);       // 发送指令
    BH1750_IIC_Wait_Ack();               // 等待从机应答
    BH1750_IIC_Stop();                   // IIC停止信号
}

u16 bh_data_read(void)
{
    u16 buf = 0;

    BH1750_IIC_Start();                  // IIC起始信号
    BH1750_IIC_Send_Byte(BHAddRead);     // 发送器件地址+读标志位
    BH1750_IIC_Wait_Ack();               // 等待从机应答

    // 读取高8位数据
    buf = BH1750_IIC_Read_Byte(1);       // 读取并发送ACK
    buf = buf << 8;                      // 左移8位

    // 读取低8位数据
    buf |= BH1750_IIC_Read_Byte(0);      // 读取并发送NACK

    BH1750_IIC_Stop();                   // 发送停止信号
    return buf;
}

void BH1750_IIC_Write_One_Byte(u8 daddr, u8 addr, u8 data)
{
    do {
        BH1750_IIC_Start();              // IIC起始信号
        BH1750_IIC_Send_Byte(daddr);     // 发送设备地址
    } while(BH1750_IIC_Wait_Ack());      // 等待应答

    BH1750_IIC_Send_Byte(addr);          // 发送寄存器地址
    BH1750_IIC_Wait_Ack();
    BH1750_IIC_Send_Byte(data);          // 发送数据
    BH1750_IIC_Wait_Ack();
    BH1750_IIC_Stop();                   // IIC停止信号
}

u8 BH1750_IIC_Read_One_Byte(u8 daddr, u8 addr)
{
    u8 data;

    // 先发送设备地址和寄存器地址
    do {
        BH1750_IIC_Start();              // IIC起始信号
        BH1750_IIC_Send_Byte(daddr);     // 发送设备地址（写）
    } while(BH1750_IIC_Wait_Ack());      // 等待应答

    BH1750_IIC_Send_Byte(addr);          // 发送寄存器地址
    BH1750_IIC_Wait_Ack();
    BH1750_IIC_Stop();                   // IIC停止信号

    // 重新开始读取数据
    BH1750_IIC_Start();                  // IIC起始信号
    BH1750_IIC_Send_Byte(daddr | 0x01);  // 发送设备地址（读）
    BH1750_IIC_Wait_Ack();

    data = BH1750_IIC_Read_Byte(0);      // 读取数据，发送NACK

    BH1750_IIC_Stop();                   // IIC停止信号
    return data;
}


void task_bh1750_proc(void){
    // 1. 从BH1750读取原始数据（假设函数）
    uint32_t raw_lux = bh_data_read(); // 你的读取函数

    // 2. 应用低通滤波
    light_level = LowPassFilter_ProcessInt(&lux_filter, raw_lux);

    // 3. PID计算
    float pid_output = PID_Update(&light_pid, light_level, 200);

    // 4. 转换为PWM占空比
    light_duty = (uint8_t)pid_output;  // 0-100
    // 5. 限制范围
    if(light_duty > 100) light_duty = 100;
    if(light_duty < 1) light_duty = 1;  // 保持最小亮度

    PWM_SetPercent(light_duty);

}
