#include "bsp_sys_timer.h"
#include "bsp_sg90.h"
#include "bsp_motor.h"
#include "ringbuffer.h"
#include "comm_protocol.h"
#include "pid.h"
#include "task_camera_ctrl.h"

#include <stdio.h>
#include <string.h>
#include <math.h>

#define IMAGE_WIDTH  320
#define IMAGE_HEIGHT 224

#define SERVO_X_INIT_VAL 90
#define SERVO_Y_INIT_VAL 80

#define FILTER_FACTOR 0.01

typedef enum {
    E_PACKET_GET,
    E_SERVO_CTRL,
    E_MOTOR_CTRL,
} camera_state_t;
static camera_state_t _task_state = E_PACKET_GET;

/* rbuffer_camera 在 bsp_uart.c 中定义 */
extern rbuffer_t rbuffer_camera;

PID pid_servo_x, pid_servo_y;

/*
 * 视觉模块数据处理初始化
 */
void task_camera_ctrl_init(void)
{
    sg90_servo_init();
    set_servo_angle(0, SERVO_X_INIT_VAL);
    set_servo_angle(1, SERVO_Y_INIT_VAL);

    pid_init(0.102, 0.016, 0, &pid_servo_x);
    pid_servo_x.setValue = IMAGE_WIDTH / 2.0;

    pid_init(-0.102, -0.016, 0, &pid_servo_y);
    pid_servo_y.setValue = IMAGE_HEIGHT / 2.0;
}

/*
 * 视觉模块数据处理
 */
void task_camera_ctrl(void)
{
    static int32_t x = 0, y = 0, width = 0, height = 0;
    static int32_t dir = 0, speed = 0;

    switch (_task_state) {
        case E_PACKET_GET: {
            int32_t rc = -1;
            uint32_t redundant = 0x00, packet_len = 0;
            uint8_t buffer[128] = {0x00};
            uint8_t payload[32] = {0x00};

            uint32_t len = rbuffer_data_len(&rbuffer_camera);
            if (len > 0) {
                len = (len <= sizeof(buffer)) ? len : sizeof(buffer);

                rbuffer_peek(&rbuffer_camera, buffer, len);
                rc = packet_check_valid(buffer, len, &redundant);
                if (redundant > 0) {
                    rbuffer_del(&rbuffer_camera, redundant);
                }

                if (rc >= 0) {
                    packet_decode(&buffer[redundant], len - redundant, payload, sizeof(payload));
                    int32_t cmd_type = 0;
                    int32_t index = 0;
                    memcpy(&cmd_type, &payload[index], 4);
                    index += 4;
                    if (cmd_type == 0x02) {
                        memcpy(&x, &payload[index], 4);
                        index += 4;
                        memcpy(&y, &payload[index], 4);
                        index += 4;
                        memcpy(&width, &payload[index], 4);
                        index += 4;
                        memcpy(&height, &payload[index], 4);
                        index += 4;
                        _task_state = E_SERVO_CTRL;
                    } else if (cmd_type == 0x01) {
                        memcpy(&dir, &payload[index], 4);
                        index += 4;
                        memcpy(&speed, &payload[index], 4);
                        index += 4;
                        _task_state = E_MOTOR_CTRL;
                    }

                    packet_len = packet_length(&buffer[redundant], len - redundant);
                    rbuffer_del(&rbuffer_camera, packet_len);
                } else if ((rc == -3) || (rbuffer_status(&rbuffer_camera) == RB_FULL)) {
                    rbuffer_del(&rbuffer_camera, len);
                }
            }
        } break;
        case E_SERVO_CTRL: {
            static double target_x = SERVO_X_INIT_VAL, target_y = SERVO_Y_INIT_VAL;

            static double valx_last = IMAGE_WIDTH / 2.0, valy_last = IMAGE_HEIGHT / 2.0;
            double valx_now = x + width / 2.0;
            double valy_now = y + height / 2.0;

            /* 一阶低通滤波 */
            pid_servo_x.actualValue = valx_now * FILTER_FACTOR + valx_last * (1 - FILTER_FACTOR);
            valx_last = valx_now;

            pid_servo_y.actualValue = valy_now * FILTER_FACTOR + valy_last * (1 - FILTER_FACTOR);
            valy_last = valy_now;

            /* 计算 PID 增量 */
            double increment_x = pid_incremental(&pid_servo_x);
            target_x += increment_x;

            double increment_y = pid_incremental(&pid_servo_y);
            target_y += increment_y;

            /* 设置舵机角度 */
            set_servo_angle(0, target_x);
            set_servo_angle(1, target_y);

            _task_state = E_PACKET_GET;
        } break;
        case E_MOTOR_CTRL: {
            motor_move((dir_t)dir, speed);
            _task_state = E_PACKET_GET;
        } break;
        default:
            break;
    }
}
