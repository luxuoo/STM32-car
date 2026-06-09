#include "bsp_sys_timer.h"

static volatile uint32_t _sys_tick = 0;

/*
 * 系统定时器初始化
 * TIM7 已由 CubeMX 配置为 1Hz (PSC=8399, ARR=9999)
 * 此处只需启动中断
 */
void sys_timer_init(void)
{
    HAL_TIM_Base_Start_IT(&htim7);
}

/*
 * 获取当前系统时钟计数值
 */
uint32_t get_sys_tick(void)
{
    return _sys_tick;
}

/*
 * 定时器溢出回调
 * 在 stm32f4xx_it.c 的 HAL_TIM_IRQHandler 中被调用
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM6) {
        HAL_IncTick();  /* HAL 系统时钟（TIM6 做 timebase） */
    }
    if (htim->Instance == TIM7) {
        _sys_tick++;    /* 用户 1Hz 系统时钟 */
    }
}
