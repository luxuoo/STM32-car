#include "board.h"

/*
 * 板级初始化
 * 注意：HAL_Init() 和 SystemClock_Config() 已在 main.c 中调用
 * 此处只做额外的初始化工作
 */
void board_init(void)
{
    /* NVIC 向量表设置 */
#define NVIC_VTOR_MASK 0x3FFFFF80
#ifdef VECT_TAB_RAM
    SCB->VTOR = (0x10000000 & NVIC_VTOR_MASK);
#else
    SCB->VTOR = (0x08000000 & NVIC_VTOR_MASK);
#endif

    /*
     * 配置 SysTick 用于微秒级延时
     * HAL 已经配置了 SysTick 产生 1ms 中断（用于 HAL_Delay）
     * 这里设置 LOAD 为最大值，delay_us() 会自己轮询 VAL
     */
    SysTick->LOAD = 0x00FFFFFF;
    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
}

/*
 * 微秒级延时（基于 SysTick 轮询）
 */
void delay_us(uint32_t _us)
{
    uint32_t ticks;
    uint32_t told, tnow, tcnt = 0;

    ticks = _us * (SystemCoreClock / 1000000);
    told = SysTick->VAL;

    while (1) {
        tnow = SysTick->VAL;
        if (tnow != told) {
            if (tnow < told)
                tcnt += told - tnow;
            else
                tcnt += SysTick->LOAD - tnow + told;

            told = tnow;

            if (tcnt >= ticks)
                break;
        }
    }
}

/*
 * 毫秒级延时
 */
void delay_ms(uint32_t _ms) { delay_us(_ms * 1000); }
void delay_1ms(uint32_t ms) { delay_us(ms * 1000); }
void delay_1us(uint32_t us) { delay_us(us); }
