#include "bsp_sys_timer.h"
#include "bsp_motor.h"
#include "task_camera_ctrl.h"

typedef enum {
    E_INIT,
    E_RUN,
} motor_state_t;

static motor_state_t _task_state = E_INIT;

/*
 * 电机控制任务
 */
void task_motor_ctrl(void)
{
    uint32_t tick_now = get_sys_tick();

    switch (_task_state) {
        case E_INIT: {
            motor_init();
            _task_state = E_RUN;
        } break;
        case E_RUN: {
            /* 预留 */
        } break;
        default:
            break;
    }
}
