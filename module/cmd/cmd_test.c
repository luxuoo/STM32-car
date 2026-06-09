#include "shell.h"
#include "bsp_motor.h"
#include "bsp_sg90.h"
#include "board.h"

#include <stdio.h>

/*
 * 电机测试命令
 */
int cmd_motor(int motor, int forword, int speed)
{
    motor_ctrl((motor_t)motor, forword, speed);
    return 0;
}

SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), motor, cmd_motor, to test motor);

/*
 * 小车移动测试命令
 */
int cmd_move(int dir, int speed)
{
    motor_move((dir_t)dir, speed);
    return 0;
}

SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), move, cmd_move, to test car move);

/*
 * 舵机测试命令
 */
int cmd_servo(int channel, int angle)
{
    for (uint8_t i = 0; i < 180; i++) {
        set_servo_angle(channel, i);
        delay_ms(50);
    }
    return 0;
}

SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), servo, cmd_servo, to test servo);
