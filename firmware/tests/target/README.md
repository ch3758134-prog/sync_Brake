# Target 驱动测试归档（EVB_NO_BRAKE_POWER）

本目录保存可长期回归的测试函数；`app/main.c` 仅做初始化、选择和调用。

- `test_gpio.[ch]`：复用既有 `gpio_bringup.[ch]` 的 LED/按键事件测试，
  另提供 `test_gpio_get_stats()` 供 Keil Watch 观察计数、按键电平和最近状态。
- `test_timebase.[ch]`：观察单调毫秒计数和 1s 软件周期的累计数/超期数，
  **不能仅凭此证明时基频率和抖动的硬件精度**。
- `gpio_bringup.[ch]`：已有 EVB IRQ/LED 回归测试核心；仍保留，不复制 SDK、
  不重复实现 GPIO_IRQHandler。后续新增实测函数继续放独立 `test_xxx.c/h`。

## 如何选择

在 `app/main.c` 修改 `BS_TARGET_TEST_SELECTED`：
`BS_TARGET_TEST_GPIO`（默认）、`BS_TARGET_TEST_TIMEBASE` 或 `BS_TARGET_TEST_APP`。
一个固件版本只主动运行选择的一组测试；其它模块源码保留在工程中。
Keil Watch 查看 `g_target_test_last_status`，并可在 `test_gpio_get_stats()`
或 `test_timebase_get_stats()` 打断点读取结果结构体。

## 实测状态

所有板级验证 **NOT_RUN**；请由负责人集中安排，不要求每次提交后烧录。
Host Fake 测试只能验证测试逻辑和 API 合同，不能证实电气行为、准确时钟或 IRQ。

每次新增正式源码与测试：
- 同一提交更新 CMake 和 Keil；`check_target_project.py` 会检查所有第一方
  `.c/.h/.S` 都已经在 Keil 工程中引用。
- Driver 中断由 Port 唯一实现，测试函数只能调用公共 Driver。
- 不接真实制动功率；不在测试代码中加入使能功率的后门。

CI 会用 Host 编译器分别对 main 的 APP/GPIO/TIMEBASE 三个选择进行语法编译，
但不会执行 Keil ArmClang 或实体 MCU。命令行可用 `-DBS_TARGET_TEST_SELECTED=2`
覆盖默认值；Keil 现场可以直接改 main 中的默认宏。
