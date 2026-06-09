/**
 * @file shell_port.c
 * @brief Shell 移植层（HAL 版本）
 */

#include "shell_port.h"
#include "ringbuffer.h"
#include "bsp_uart.h"

/*
 * 用户 shell 实例
 */
Shell shell_user;
static char _buffer_shell_user[512] = {0};

/*
 * shell 用户 ringbuffer（UART1 接收）
 */
rbuffer_t rbuffer_user;
static uint8_t _rbuffer_user_lowlevel[256] = {0};

/**
 * @brief shell 写函数（通过 UART1 发送）
 */
short userShellWrite(char *data, unsigned short len)
{
    HAL_UART_Transmit(&huart1, (uint8_t *)data, len, HAL_MAX_DELAY);
    return len;
}

/**
 * @brief shell 读函数（从 ringbuffer 读取）
 */
short userShellRead(char *data, unsigned short len)
{
    return rbuffer_get(&rbuffer_user, (uint8_t *)data, len);
}

/**
 * @brief shell 初始化
 */
void userShellInit(void)
{
    rbuffer_init(&rbuffer_user, _rbuffer_user_lowlevel, sizeof(_rbuffer_user_lowlevel));

    shell_user.read = userShellRead;
    shell_user.write = userShellWrite;
    shellInit(&shell_user, _buffer_shell_user, sizeof(_buffer_shell_user));
}
