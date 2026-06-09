#include "shell.h"
#include "bsp_motor.h"
#include "bsp_sg90.h"
#include "board.h"
#include "task_test.h"

#include <stdio.h>

/*
 * 电机测试命令
 * 用法: motor <motor_id> <direction> <speed>
 *   motor_id: 0=左后 1=左前 2=右后 3=右前
 *   direction: 0=反转 1=正转
 *   speed: 0~1000
 */
int cmd_motor(int motor, int forword, int speed)
{
    motor_ctrl((motor_t)motor, forword, speed);
    return 0;
}

SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), motor, cmd_motor, test single motor);

/*
 * 小车移动测试命令
 * 用法: move <direction> <speed>
 *   direction: 0=前进 1=后退 2=左转 3=右转
 *   speed: 0~1000
 */
int cmd_move(int dir, int speed)
{
    motor_move((dir_t)dir, speed);
    return 0;
}

SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), move, cmd_move, test car move);

/*
 * 舵机测试命令
 * 用法: servo <channel> <angle>
 *   channel: 0=X轴 1=Y轴
 *   angle: 0~180
 */
int cmd_servo(int channel, int angle)
{
    sg90_servo_init();
    set_servo_angle(channel, angle);
    printf("servo ch%d -> %d deg\r\n", channel, angle);
    return 0;
}

SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), servo, cmd_servo, set servo angle);

/* ======================================================================== */
/*  脱机测试命令                                                              */
/* ======================================================================== */

/*
 * 运行全部脱机测试
 * 用法: test
 */
int cmd_test(void)
{
    test_run_all();
    return 0;
}

SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), test, cmd_test, run all offline tests);

/*
 * LED 单项测试
 * 用法: test_led
 */
int cmd_test_led(void)
{
    test_led();
    return 0;
}

SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), test_led, cmd_test_led, LED blink test);

/*
 * 电机单项测试
 * 用法: test_motor
 */
int cmd_test_motor(void)
{
    test_motor();
    return 0;
}

SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), test_motor, cmd_test_motor, motor sweep test);

/*
 * 舵机单项测试
 * 用法: test_servo
 */
int cmd_test_servo(void)
{
    test_servo();
    return 0;
}

SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), test_servo, cmd_test_servo, servo sweep test);

/*
 * 通信协议编解码测试
 * 用法: test_proto
 */
int cmd_test_proto(void)
{
    test_protocol();
    return 0;
}

SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), test_proto, cmd_test_proto, protocol encode/decode test);

/*
 * PID 算法测试
 * 用法: test_pid
 */
int cmd_test_pid(void)
{
    test_pid();
    return 0;
}

SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), test_pid, cmd_test_pid, PID convergence test);

/*
 * UART2 回显测试
 * 用法: test_uart
 */
int cmd_test_uart(void)
{
    test_uart_echo();
    return 0;
}

SHELL_EXPORT_CMD(SHELL_CMD_PERMISSION(0) | SHELL_CMD_TYPE(SHELL_TYPE_CMD_FUNC), test_uart, cmd_test_uart, UART2 echo test);
