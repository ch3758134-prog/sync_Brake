cmake_minimum_required(VERSION 3.16)
project(BrakeSyncHost LANGUAGES C)

# This file is HOST ONLY. Do not use it to claim YTM32 target support.
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

add_library(brakesync_can STATIC
    bsw/drivers/can_bus.c
)
target_include_directories(brakesync_can PUBLIC bsw/drivers)

add_library(brakesync_motion STATIC
    asw/motion_sample.c
)
target_include_directories(brakesync_motion PUBLIC asw)

foreach(target brakesync_can brakesync_motion)
    if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic -Werror)
    endif()
endforeach()

include(CTest)
if(BUILD_TESTING)
    add_executable(test_can_bus tests/host/test_can_bus.c)
    target_link_libraries(test_can_bus PRIVATE brakesync_can)
    add_test(NAME can_bus_host COMMAND test_can_bus)

    add_executable(test_motion_sample tests/host/test_motion_sample.c)
    target_link_libraries(test_motion_sample PRIVATE brakesync_motion)
    add_test(NAME motion_sample_host COMMAND test_motion_sample)
endif()
