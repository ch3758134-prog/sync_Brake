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
endif()
