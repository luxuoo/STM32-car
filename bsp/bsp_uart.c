#include "bsp_uart.h"
#include "shell.h"
#include "shell_port.h"

#include <stdio.h>

static rbuffer_t _rbuffer_camera_static;
rbuffer_t rbuffer_camera;
static uint8_t _rbuffer_camera_lowlevel[256] = {0};

/* UART 接收缓冲区 */
static uint8_t _uart1_rx_byte;
static uint8_t _uart2_rx_byte;

/*
 * UART1 初始化 (PA9-TX, PA10-RX)
 * CubeMX 已经配置了引脚和基本参数，这里开启中断接收
 */
void uart1_init(uint32_t baudrate)
{
    /* CubeMX 已完成 huart1 初始化，此处只需启动中断接收 */
    rbuffer_camera = _rbuffer_camera_static;
    HAL_UART_Receive_IT(&huart1, &_uart1_rx_byte, 1);
}

/*
 * UART2 初始化 (PD5-TX, PD6-RX)
 */
void uart2_init(uint32_t baudrate)
{
    /* CubeMX 已完成 huart2 初始化，此处只需启动中断接收 */
    HAL_UART_Receive_IT(&huart2, &_uart2_rx_byte, 1);
}

/*
 * HAL UART 接收完成回调函数
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1)
    {
        char data_recv = (char)_uart1_rx_byte;
        rbuffer_putchar(&rbuffer_user, data_recv);
        HAL_UART_Receive_IT(&huart1, &_uart1_rx_byte, 1);
    }
    else if (huart->Instance == USART2)
    {
        char data_recv = (char)_uart2_rx_byte;
        rbuffer_putchar(&rbuffer_camera, data_recv);
        HAL_UART_Receive_IT(&huart2, &_uart2_rx_byte, 1);
    }
}

/*
 * printf 重定向到 UART1 (HAL 版本)
 */
int fputc(int ch, FILE *f)
{
    HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
    return ch;
}
