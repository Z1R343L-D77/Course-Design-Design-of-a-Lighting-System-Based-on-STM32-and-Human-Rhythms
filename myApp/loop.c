/**
  ******************************************************************************
  * @file           : loop.c
  * @author         : 19816
  * @brief          : None
  * @attention      : None
  * @date           : 2025/1/20
  ******************************************************************************
  */
#include "bsp_system.h"
void loop(void)
{
    SystickInit();
    nGpio_init();
    myrtc_init();
    myadc_init();
    mylcd_init();
    button_init_all();


    BH1750_Init();
    WS2812_Init();
    myPWM_Init();


    scheduler_init();
    HAL_Delay(1000);
    while(1)
    {
        scheduler_run();
    }
}
