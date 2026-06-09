# STM32 智能小车项目

基于 STM32F407 的视觉追踪智能小车，支持摄像头目标追踪、四轮电机驱动和串口调试。

## 硬件平台

| 项目 | 型型/参数 |
|------|----------|
| MCU | STM32F407VGT6 (168MHz, Cortex-M4F) |
| 电机驱动 | 4路直流电机 (左前/左后/右前/右后) |
| 舵机 | 2路 SG90 (X/Y轴，用于云台追踪) |
| 视觉模块 | 串口摄像头 (UART2, 115200bps) |
| 调试串口 | UART1 (115200bps, letter-shell) |
| 指示灯 | PB2 LED |

## 系统时钟

```
HSE = 8MHz → PLL → SYSCLK = 168MHz
APB1 = 42MHz (TIM3/4/5/7)
APB2 = 84MHz (USART1/2)
```

## 软件架构

```
├── Core/               # STM32CubeMX 生成代码
│   ├── Src/main.c      # 主循环
│   ├── gpio.c          # GPIO 配置
│   ├── tim.c           # 定时器配置 (TIM3/4/5/7)
│   └── usart.c         # 串口配置 (USART1/2)
│
├── app/                # 应用层任务
│   ├── task_idle.c     # 空闲任务 (LED闪烁 + Shell)
│   ├── task_motor_ctrl.c   # 电机控制任务
│   └── task_camera_ctrl.c  # 视觉追踪任务 (核心)
│
├── bsp/                # 板级支持包
│   ├── bsp_motor.c     # 四轮电机驱动 (PWM)
│   ├── bsp_sg90.c      # SG90舵机驱动 (PWM)
│   ├── bsp_uart.c      # 串口驱动 (DMA接收)
│   └── bsp_sys_timer.c # 系统定时器
│
├── module/             # 软件模块
│   ├── pid.c           # PID 控制算法
│   ├── ringbuffer.c    # 环形缓冲区
│   ├── comm_protocol.c # 通信协议编解码
│   ├── letter-shell/   # 命令行调试工具
│   └── cmd/            # Shell 命令定义
│
├── board/              # 板级初始化
│   └── board.c         # 板级统一初始化入口
│
└── Drivers/            # ST HAL/LL 驱动库
```

## 核心功能

### 1. 视觉追踪 (task_camera_ctrl)

接收视觉模块通过 UART2 发送的目标坐标数据，使用 **增量式PID算法** 控制云台舵机追踪目标：

```
视觉模块 → UART2 → 环形缓冲区 → 协议解析 → PID计算 → 舵机输出
```

- 一阶低通滤波平滑坐标输入
- X/Y 轴独立 PID 控制
- 支持目标丢失后保持位置

### 2. 电机控制 (task_motor_ctrl)

四轮差速驱动，支持前进/后退/左转/右转：

```c
motor_move(E_DIR_FORWORD, speed);   // 前进
motor_move(E_DIR_BACKWORD, speed);  // 后退
motor_move(E_DIR_LEFT, speed);      // 左转
motor_move(E_DIR_RIGHT, speed);     // 右转
```

### 3. 通信协议 (comm_protocol)

自定义二进制协议，支持：
- `0x01` — 电机控制指令 (方向 + 速度)
- `0x02` — 视觉追踪指令 (x, y, width, height)

### 4. 调试终端 (letter-shell)

通过 UART1 提供交互式命令行，支持：
- 命令自动补全
- 历史记录
- 变量查看/修改
- 自定义测试命令

## 构建

### 环境要求

- STM32CubeCLT / GNU Tools for STM32 (arm-none-eabi-gcc ≥ 14.x)
- CMake ≥ 3.22

### 编译

```bash
# 生成构建文件
cmake -B build/Debug -DCMAKE_BUILD_TYPE=Debug

# 编译
cmake --build build/Debug
```

### 烧录

```bash
# 使用 STM32CubeProgrammer
STM32_Programmer_CLI -c port=SWD -w build/Debug/bate2.elf -v -rst
```

## 引脚分配

| 外设 | 引脚 | 功能 |
|------|------|------|
| TIM3 | CH1-CH4 | 电机 PWM |
| TIM4 | CH1-CH2 | 舵机 PWM |
| TIM5 | CH1-CH4 | 电机方向控制 |
| TIM7 | — | 系统定时基准 |
| USART1 | PA9/PA10 | 调试串口 (Shell) |
| USART2 | PA2/PA3 | 视觉模块通信 |
| PB2 | GPIO Output | LED 指示灯 |

## 目录结构说明

```
bate2/
├── CMakeLists.txt          # 主 CMake 构建文件
├── STM32F407XX_FLASH.ld    # 链接脚本 (含 shellCommand section)
├── CMakePresets.json        # CMake 预设配置
├── bate2.ioc               # STM32CubeMX 工程文件
└── README.md               # 本文档
```

## License

STM32 HAL Driver: STMicroelectronics BSD License
Letter Shell: MIT License
