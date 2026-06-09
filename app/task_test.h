#ifndef __TASK_TEST_H__
#define __TASK_TEST_H__

#include <stdint.h>

/**
 * @brief 脱机测试模块
 *
 * 通过 Shell 命令逐项测试各硬件外设，验证小车基础功能是否正常。
 * 使用方法: 在 letter-shell 中输入对应测试命令。
 *
 * 支持的测试命令:
 *   test all      - 运行全部测试
 *   test led      - LED 闪烁测试
 *   test motor    - 四轮电机逐个转动测试
 *   test servo    - 舵机角度扫描测试
 *   test protocol - 通信协议编解码回环测试
 *   test pid      - PID 算法收敛测试
 *   test uart     - 串口回显测试 (UART2)
 */

/**
 * @brief 运行全部测试
 */
void test_run_all(void);

/**
 * @brief LED 闪烁测试
 *        PB2 LED 快速闪烁 5 次
 */
void test_led(void);

/**
 * @brief 四轮电机测试
 *        依次驱动 左后→左前→右后→右前，每轮正转 500ms 停 200ms
 */
void test_motor(void);

/**
 * @brief 舵机扫描测试
 *        X/Y 轴舵机从 0°→180°→0° 扫描
 */
void test_servo(void);

/**
 * @brief 通信协议编解码回环测试
 *        构造电机指令和视觉指令数据包，编码后解码验证一致性
 */
void test_protocol(void);

/**
 * @brief PID 算法收敛测试
 *        设定目标值，迭代运行增量式 PID，验证输出趋近目标
 */
void test_pid(void);

/**
 * @brief UART2 回显测试
 *        从 UART2 收到的数据原样发回，持续 5 秒
 */
void test_uart_echo(void);

#endif /* __TASK_TEST_H__ */
