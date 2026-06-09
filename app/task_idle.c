#include "bsp_sys_timer.h"
#include "task_idle.h"
#include "shell_port.h"
#include "main.h"

/*
 * 空闲任务初始化
 */
void task_idle_init(void)
{
    /* LED 引脚初始化已由 CubeMX 完成（PB2） */
    /* Shell 初始化 */
    userShellInit();
}

/*
 * 空闲任务
 */
void task_idle(void)
{
    static uint32_t tick_last = 0;
    uint32_t tick_now = get_sys_tick();

    /*
     * 每秒翻转 LED 指示灯状态
     */
    if (tick_now - tick_last >= 1) {
        tick_last = tick_now;
        HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
    }

    /*
     * 处理 shell 命令
     */
    shellTask(&shell_user);
}
