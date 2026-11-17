# Board｜板级配置

`ytm32b1le0/generated/` 保留 YCT 示例产生的 PinMux、Clock、Startup、Scatter 等文件及原 .yct，具有现有 EVB 的硬件参考价值。

`ytm32b1le0/board_support.[ch]` 是自研板级初始化代码。GPIO 自测的 ISR/LED 不在本目录，而在 `tests/target/gpio_bringup.c`。

目标 MCU YTM32B1LE05 / EVB 引脚仅适用于上传示例。最终产品 PCB 的上电、功率禁止、电源和保护路径仍需要硬件安全设计。
