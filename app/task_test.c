#include "task_test.h"
#include "bsp_motor.h"
#include "bsp_sg90.h"
#include "bsp_sys_timer.h"
#include "comm_protocol.h"
#include "pid.h"
#include "ringbuffer.h"
#include "main.h"

#include <stdio.h>
#include <string.h>

/* UART2 接收缓冲区 (bsp_uart.c 中定义) */
extern rbuffer_t rbuffer_camera;
extern UART_HandleTypeDef huart2;

/* 测试结果统计 */
static uint32_t _pass_cnt = 0;
static uint32_t _fail_cnt = 0;

static void _print_header(const char *name)
{
    printf("\r\n========== [TEST] %s ==========\r\n", name);
}

static void _print_result(const char *item, int pass)
{
    if (pass) {
        _pass_cnt++;
        printf("  [PASS] %s\r\n", item);
    } else {
        _fail_cnt++;
        printf("  [FAIL] %s\r\n", item);
    }
}

static void _print_summary(void)
{
    printf("\r\n---------- 测试结果: %lu PASS / %lu FAIL ----------\r\n",
           _pass_cnt, _fail_cnt);
}

/* ========================================================================== */
/*  LED 测试                                                                   */
/* ========================================================================== */
void test_led(void)
{
    _print_header("LED 闪烁测试");
    printf("  PB2 LED 快速闪烁 5 次，请观察...\r\n");

    for (int i = 0; i < 5; i++) {
        HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
        HAL_Delay(150);
        HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
        HAL_Delay(150);
    }
    _print_result("LED 闪烁", 1);
    printf("  >> 请确认 LED 是否闪烁 5 次\r\n");
}

/* ========================================================================== */
/*  电机测试                                                                   */
/* ========================================================================== */
void test_motor(void)
{
    _print_header("四轮电机测试");

    static const struct {
        motor_t motor;
        const char *name;
    } motors[] = {
        { E_MOTOR_LB, "左后(LB)" },
        { E_MOTOR_LF, "左前(LF)" },
        { E_MOTOR_RB, "右后(RB)" },
        { E_MOTOR_RF, "右前(RF)" },
    };

    motor_init();
    HAL_Delay(200);

    for (int i = 0; i < 4; i++) {
        printf("  驱动 %s 正转 500ms (speed=400)...\r\n", motors[i].name);
        motor_ctrl(motors[i].motor, 1, 400);
        HAL_Delay(500);
        motor_ctrl(motors[i].motor, 0, 0);
        HAL_Delay(200);
        _print_result(motors[i].name, 1);
    }

    /* 整车运动测试 */
    static const struct {
        dir_t dir;
        const char *name;
    } dirs[] = {
        { E_DIR_FORWORD,  "前进" },
        { E_DIR_BACKWORD, "后退" },
        { E_DIR_LEFT,     "左转" },
        { E_DIR_RIGHT,    "右转" },
    };

    for (int i = 0; i < 4; i++) {
        printf("  整车%s 300ms...\r\n", dirs[i].name);
        motor_move(dirs[i].dir, 400);
        HAL_Delay(300);
        motor_move(dirs[i].dir, 0);
        HAL_Delay(200);
        _print_result(dirs[i].name, 1);
    }
    printf("  >> 请确认电机是否按顺序转动\r\n");
}

/* ========================================================================== */
/*  舵机测试                                                                   */
/* ========================================================================== */
void test_servo(void)
{
    _print_header("舵机扫描测试");

    sg90_servo_init();

    /* X 轴扫描 */
    printf("  X 轴 0→180°...\r\n");
    for (double a = 0; a <= 180; a += 5) {
        set_servo_angle(0, a);
        HAL_Delay(30);
    }
    printf("  X 轴 180→0°...\r\n");
    for (double a = 180; a >= 0; a -= 5) {
        set_servo_angle(0, a);
        HAL_Delay(30);
    }
    set_servo_angle(0, 90);
    _print_result("X 轴扫描", 1);

    /* Y 轴扫描 */
    printf("  Y 轴 0→180°...\r\n");
    for (double a = 0; a <= 180; a += 5) {
        set_servo_angle(1, a);
        HAL_Delay(30);
    }
    printf("  Y 轴 180→0°...\r\n");
    for (double a = 180; a >= 0; a -= 5) {
        set_servo_angle(1, a);
        HAL_Delay(30);
    }
    set_servo_angle(1, 80);
    _print_result("Y 轴扫描", 1);

    printf("  >> 请确认舵机是否来回扫描\r\n");
}

/* ========================================================================== */
/*  通信协议编解码测试                                                          */
/* ========================================================================== */
void test_protocol(void)
{
    _print_header("通信协议编解码回环测试");

    int pass = 1;

    /* 测试 1: 电机控制指令 (cmd=0x01, dir=1, speed=500) */
    {
        uint8_t payload[8];
        int32_t cmd = 0x01, dir = 1, speed = 500;
        int idx = 0;
        memcpy(&payload[idx], &cmd, 4);   idx += 4;
        memcpy(&payload[idx], &dir, 4);   idx += 4;
        /* 电机指令 payload 实际是 cmd(4) + dir(4) = 8, 但 speed 另外处理 */
        /* 按 task_camera_ctrl 的解码方式: cmd_type(4) + dir(4) + speed(4) = 12 */
        uint8_t payload1[12];
        memcpy(&payload1[0], &cmd, 4);
        memcpy(&payload1[4], &dir, 4);
        memcpy(&payload1[8], &speed, 4);

        uint8_t packet[32] = {0};
        int32_t pkt_len = packet_encode(payload1, sizeof(payload1), packet, sizeof(packet));
        if (pkt_len < 0) {
            _print_result("电机指令编码", 0);
            pass = 0;
        } else {
            uint32_t redundant = 0;
            int32_t rc = packet_check_valid(packet, pkt_len, &redundant);
            if (rc != 0) {
                _print_result("电机指令校验", 0);
                pass = 0;
            } else {
                uint8_t decoded[32] = {0};
                int32_t dec_len = packet_decode(packet, pkt_len, decoded, sizeof(decoded));
                if (dec_len != sizeof(payload1) || memcmp(decoded, payload1, dec_len) != 0) {
                    _print_result("电机指令解码一致性", 0);
                    pass = 0;
                } else {
                    _print_result("电机指令编解码", 1);
                }
            }
        }
    }

    /* 测试 2: 视觉追踪指令 (cmd=0x02, x=160, y=112, w=80, h=60) */
    {
        int32_t cmd = 0x02, x = 160, y = 112, w = 80, h = 60;
        uint8_t payload2[20];
        memcpy(&payload2[0],  &cmd, 4);
        memcpy(&payload2[4],  &x,   4);
        memcpy(&payload2[8],  &y,   4);
        memcpy(&payload2[12], &w,   4);
        memcpy(&payload2[16], &h,   4);

        uint8_t packet[32] = {0};
        int32_t pkt_len = packet_encode(payload2, sizeof(payload2), packet, sizeof(packet));
        if (pkt_len < 0) {
            _print_result("视觉指令编码", 0);
            pass = 0;
        } else {
            uint32_t redundant = 0;
            int32_t rc = packet_check_valid(packet, pkt_len, &redundant);
            if (rc != 0) {
                _print_result("视觉指令校验", 0);
                pass = 0;
            } else {
                uint8_t decoded[32] = {0};
                int32_t dec_len = packet_decode(packet, pkt_len, decoded, sizeof(decoded));
                if (dec_len != sizeof(payload2) || memcmp(decoded, payload2, dec_len) != 0) {
                    _print_result("视觉指令解码一致性", 0);
                    pass = 0;
                } else {
                    _print_result("视觉指令编解码", 1);
                }
            }
        }
    }

    /* 测试 3: 帧头损坏检测 */
    {
        uint8_t bad_pkt[] = {0xBB, 0x04, 0x00, 0x01, 0x02, 0x03, 0x04, 0x00, 0x55};
        uint32_t redundant = 0;
        int32_t rc = packet_check_valid(bad_pkt, sizeof(bad_pkt), &redundant);
        if (rc != -3 && rc != -2) {
            _print_result("坏帧头检测", 0);
            pass = 0;
        } else {
            _print_result("坏帧头检测", 1);
        }
    }

    if (pass) {
        printf("  >> 协议编解码功能正常\r\n");
    }
}

/* ========================================================================== */
/*  PID 算法测试                                                               */
/* ========================================================================== */
void test_pid(void)
{
    _print_header("PID 算法收敛测试");

    PID pid;
    pid_init(0.5, 0.1, 0.05, &pid);
    pid.setValue = 100.0;
    pid.actualValue = 0.0;

    printf("  目标值: %.1f, 初始值: %.1f\r\n", pid.setValue, pid.actualValue);
    printf("  迭代 50 次增量式 PID...\r\n");

    int converged = 0;
    for (int i = 0; i < 50; i++) {
        double inc = pid_incremental(&pid);
        pid.actualValue += inc;

        if (i % 10 == 0) {
            printf("    iter %2d: actual=%.2f, inc=%.4f\r\n", i, pid.actualValue, inc);
        }

        /* 检查是否收敛到目标值 ±5% */
        if (pid.actualValue > 95.0 && pid.actualValue < 105.0) {
            converged = 1;
        }
    }

    printf("  最终值: %.2f\r\n", pid.actualValue);
    _print_result("PID 收敛 (目标100, ±5%)", converged);
    printf("  >> PID 算法%s\r\n", converged ? "正常" : "异常");
}

/* ========================================================================== */
/*  UART2 回显测试                                                             */
/* ========================================================================== */
void test_uart_echo(void)
{
    _print_header("UART2 回显测试");
    printf("  UART2 回显模式，持续 5 秒...\r\n");
    printf("  请从 UART2 对端发送数据，本设备将原样回发\r\n");

    uint32_t tick_start = HAL_GetTick();
    uint32_t echo_cnt = 0;

    while ((HAL_GetTick() - tick_start) < 5000) {
        uint8_t ch;
        if (rbuffer_getchar(&rbuffer_camera, &ch) == 1) {
            HAL_UART_Transmit(&huart2, &ch, 1, 100);
            echo_cnt++;
        }
    }

    printf("  回显字节数: %lu\r\n", echo_cnt);
    _print_result("UART2 回显", 1);
    printf("  >> 已退出回显模式\r\n");
}

/* ========================================================================== */
/*  运行全部测试                                                               */
/* ========================================================================== */
void test_run_all(void)
{
    _pass_cnt = 0;
    _fail_cnt = 0;

    printf("\r\n");
    printf("╔══════════════════════════════════════╗\r\n");
    printf("║      STM32 智能小车 脱机测试         ║\r\n");
    printf("╚══════════════════════════════════════╝\r\n");

    test_led();
    test_motor();
    test_servo();
    test_protocol();
    test_pid();
    test_uart_echo();

    _print_summary();
}
