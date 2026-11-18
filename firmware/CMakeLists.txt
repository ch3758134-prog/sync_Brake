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
    asw/motion_sample.c
)
target_include_directories(brakesync_motion PUBLIC asw)

add_library(brakesync_time_service STATIC
    bsw/services/time_service.c
)
target_include_directories(brakesync_time_service PUBLIC
    bsw/services bsw/mcal/include
)

foreach(target brakesync_can_sample brakesync_motion brakesync_time_service)
    if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic -Werror)
    endif()
endforeach()

add_library(brakesync_portable_core STATIC
    asw/brake_intent.c
    asw/motion_signal.c
    asw/diagnostics.c
    app/app_scheduler.c
    asw/run_qualification.c
    asw/output_authorization.c
)
target_include_directories(brakesync_portable_core PUBLIC
    common asw app
)
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(brakesync_portable_core PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

add_library(brakesync_can_frame STATIC
    bsw/drivers/drv_can_frame.c
)
target_include_directories(brakesync_can_frame PUBLIC
    common bsw/drivers
)
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(brakesync_can_frame PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

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
    target_include_directories(test_driver_can_frame PRIVATE bsw/cdd)
    target_link_libraries(test_driver_can_frame PRIVATE brakesync_can_frame)
    add_test(NAME test_driver_can_frame COMMAND test_driver_can_frame)
endif()
