cmake_minimum_required(VERSION 3.16)
project(BrakeSyncHost LANGUAGES C)

# HOST ONLY: do not link vendor MCU SDK, real power output or Target IRQ.
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

add_library(brakesync_can_sample STATIC
    tests/host/can_sample/can_bus.c
)
target_include_directories(brakesync_can_sample PUBLIC
    tests/host/can_sample
)

add_library(brakesync_motion STATIC
    source/ASW/Motion/motion_sample.c
)


add_library(brakesync_time_service STATIC
    source/BSW/Services/Time/time_service.c
)


foreach(target brakesync_can_sample brakesync_motion brakesync_time_service)
    if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic -Werror)
    endif()
endforeach()

add_library(brakesync_portable_core STATIC
    source/ASW/BrakeInput/brake_intent.c
    source/ASW/Motion/motion_signal.c
    source/ASW/Diagnostics/diagnostics.c
    source/BSW/Services/Scheduler/app_scheduler.c
    source/ASW/SystemState/run_qualification.c
    source/ASW/SystemState/output_authorization.c
)

if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(brakesync_portable_core PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

add_library(brakesync_can_frame STATIC
    source/BSW/MCAL/CAN/drv_can_frame.c
)

if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(brakesync_can_frame PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

# Hardware-independent Driver buffering and bounded diagnostic event logging.
add_library(brakesync_bsw_core STATIC
    source/BSW/Services/Buffer/drv_byte_fifo.c
    source/BSW/Services/EventLog/diag_log.c
    source/BSW/Services/Health/health_service.c
)

if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(brakesync_bsw_core PRIVATE
        -Wall -Wextra -Wpedantic -Werror)
endif()

set(BS_PUBLIC_HEADERS
    source
    tests/target
    platform/ytm32b1le0/config
    platform/ytm32b1le0/startup
    platform/ytm32b1le0/board_port
    platform/ytm32b1le0/mcal_port
    source/BSW/Services/Scheduler
    source/ASW/BrakeInput
    source/ASW/BrakeControl
    source/ASW/Diagnostics
    source/ASW/Motion
    source/ASW/SystemState
    source/BSW/EAL/BoardIO
    source/BSW/EAL/Bluetooth/WLT8016
    source/BSW/EAL/MotionSensor/BMA510
    source/BSW/EAL/MotionSensor
    source/BSW/EAL/MotionSensor/CanImu
    source/BSW/MCAL/ADC
    source/BSW/Services/Buffer
    source/BSW/MCAL/CAN
    source/BSW/MCAL/System
    source/BSW/MCAL/I2C
    source/BSW/MCAL/SPI
    source/BSW/MCAL/Timer
    source/BSW/MCAL/Common
    source/BSW/MCAL/UART
    source/BSW/Services/EventLog
    source/BSW/Services/Health
    source/BSW/Services/Time
    source/Common
)
foreach(t brakesync_can_sample brakesync_motion brakesync_time_service
          brakesync_portable_core brakesync_can_frame brakesync_bsw_core)
    target_include_directories(${t} PUBLIC ${BS_PUBLIC_HEADERS})
endforeach()
include(CTest)
if(BUILD_TESTING)
    add_executable(test_can_bus tests/host/test_can_bus.c)
    target_link_libraries(test_can_bus PRIVATE brakesync_can_sample)
    add_test(NAME can_bus_host_sample COMMAND test_can_bus)

    add_executable(test_motion_sample tests/host/test_motion_sample.c)
    target_link_libraries(test_motion_sample PRIVATE brakesync_motion)
    add_test(NAME motion_sample_host COMMAND test_motion_sample)

    add_executable(test_time_service tests/host/test_time_service.c)
    target_link_libraries(test_time_service PRIVATE brakesync_time_service)
    add_test(NAME time_service_host COMMAND test_time_service)
    foreach(test_name test_brake_intent test_motion_signal test_diagnostics test_app_scheduler)
        add_executable(${test_name} tests/host/${test_name}.c)
        target_link_libraries(${test_name} PRIVATE brakesync_portable_core)
        add_test(NAME ${test_name} COMMAND ${test_name})
    endforeach()
    add_executable(test_qualification tests/host/test_qualification.c)
    target_link_libraries(test_qualification PRIVATE brakesync_portable_core)
    add_test(NAME test_qualification COMMAND test_qualification)
    add_executable(test_driver_can_frame tests/host/test_driver_can_frame.c)
    
    target_link_libraries(test_driver_can_frame PRIVATE brakesync_can_frame)
    add_test(NAME test_driver_can_frame COMMAND test_driver_can_frame)
    add_executable(test_target_gpio
        tests/host/test_target_gpio.c
        tests/target/test_gpio.c
        tests/target/gpio_bringup.c
    )
    
    target_include_directories(test_target_gpio PRIVATE ${BS_PUBLIC_HEADERS})
    add_test(NAME test_target_gpio COMMAND test_target_gpio)

    add_executable(test_target_timebase
        tests/host/test_target_timebase.c
        tests/target/test_timebase.c
    )
    
    target_include_directories(test_target_timebase PRIVATE ${BS_PUBLIC_HEADERS})
    add_test(NAME test_target_timebase COMMAND test_target_timebase)

    # Compile all three main.c branches using identical headers.
    # OBJECT targets intentionally are not linked into Host test executables.
    foreach(main_mode RANGE 0 2)
        add_library(brakesync_main_mode_${main_mode} OBJECT source/main.c)
        target_include_directories(brakesync_main_mode_${main_mode} PRIVATE ${BS_PUBLIC_HEADERS})
        target_compile_definitions(brakesync_main_mode_${main_mode}
            PRIVATE BS_TARGET_TEST_SELECTED=${main_mode})
        
        if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
            target_compile_options(brakesync_main_mode_${main_mode}
                PRIVATE -Wall -Wextra -Wpedantic -Werror)
        endif()
    endforeach()
    if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
        foreach(t test_target_gpio test_target_timebase)
            target_compile_options(${t} PRIVATE -Wall -Wextra -Wpedantic -Werror)
        endforeach()
    endif()

    foreach(test_name test_drv_byte_fifo test_diag_log test_health_service)
        add_executable(${test_name} tests/host/${test_name}.c)
        target_link_libraries(${test_name} PRIVATE brakesync_bsw_core)
        if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
            target_compile_options(${test_name} PRIVATE
                -Wall -Wextra -Wpedantic -Werror)
        endif()
        add_test(NAME ${test_name} COMMAND ${test_name})
    endforeach()
endif()
