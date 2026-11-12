cmake_minimum_required(VERSION 3.16)
project(BrakeSyncFirmwareHost C)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
add_library(brakesync_core STATIC
  src/BSW/Driver/Port.c
  src/BSW/Driver/Gpio.c
  src/BSW/Driver/Can.c
  src/BSW/Driver/Timer.c
  src/BSW/Driver/Uart.c
  src/BSW/Driver/I2c.c
  src/BSW/Cdd/Bma510.c
  src/BSW/Cdd/Wlt8016.c
  src/ASW/BrakeSystem.c
)
target_include_directories(brakesync_core PUBLIC
  include src/BSW/Cdd src/ASW
)
if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
  target_compile_options(brakesync_core PRIVATE -Wall -Wextra -Werror -Wpedantic)
endif()
include(CTest)
if(BUILD_TESTING)
  add_executable(test_bma510 tests/host/test_bma510.c)
  target_link_libraries(test_bma510 PRIVATE brakesync_core)
  add_test(NAME bma510 COMMAND test_bma510)
  add_executable(test_driver tests/host/test_driver.c)
  target_link_libraries(test_driver PRIVATE brakesync_core)
  add_test(NAME driver COMMAND test_driver)
endif()
