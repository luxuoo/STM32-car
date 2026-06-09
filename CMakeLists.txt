cmake_minimum_required(VERSION 3.22)

#
# This file is generated only once,
# and is not re-generated if converter is called multiple times.
#
# User is free to modify the file as much as necessary
#

# Setup compiler settings
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)


# Define the build type
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

# Set the project name
set(CMAKE_PROJECT_NAME bate2)

# Enable compile command to ease indexing with e.g. clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Core project settings
project(${CMAKE_PROJECT_NAME})
message("Build type: " ${CMAKE_BUILD_TYPE})

# Enable CMake support for ASM and C languages
enable_language(C ASM)

# Create an executable object type
add_executable(${CMAKE_PROJECT_NAME})

# Add STM32CubeMX generated sources
add_subdirectory(cmake/stm32cubemx)

# Link directories setup
target_link_directories(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined library search paths
)

# Add sources to executable
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    # Application (main.c 已在 Core/Src/main.c 中)
    app/task_camera_ctrl.c
    app/task_idle.c
    app/task_motor_ctrl.c
    # Board
    board/board.c
    # BSP
    bsp/bsp_motor.c
    bsp/bsp_sg90.c
    bsp/bsp_sys_timer.c
    bsp/bsp_uart.c
    # Modules
    module/cmd/cmd_test.c
    module/comm_protocol.c
    module/letter-shell/shell.c
    module/letter-shell/shell_cmd_list.c
    module/letter-shell/shell_companion.c
    module/letter-shell/shell_ext.c
    module/letter-shell/shell_port.c
    module/pid.c
    module/ringbuffer.c
)

# Add include paths
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    app
    bsp
    board
    module
    module/letter-shell
)

# Add project symbols (macros)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    # Add user defined symbols
)

# Remove wrong libob.a library dependency when using cpp files
list(REMOVE_ITEM CMAKE_C_IMPLICIT_LINK_LIBRARIES ob)

# Add linked libraries
target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32cubemx

    # Add user defined libraries
)
