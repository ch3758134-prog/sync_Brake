# MCU 实板测试代码（长期保存）

`source/main.c` 当前保留以下编译期选择：

- `BS_TARGET_TEST_GPIO`（默认）：GPIO/按键中断测试；
- `BS_TARGET_TEST_TIMEBASE`：现有毫秒时基逻辑检查；
- `BS_TARGET_TEST_EVB_DEMO`：先前保留的开发板 GPIO 演示入口。

实际测试逻辑依然在 `test_gpio.c/h`、`gpio_bringup.c/h`、`test_timebase.c/h` 和 `evb_app.c/h` 中，切换主函数不会删除测试代码。

`legacy/` 是对**此前我们自行编写的 EVB GPIO 实验驱动**的归档。它引用 SDK `PINS_DRV_*` 和 YCT 引脚配置，当前 Keil 测试固件仍使用这份实验实现，以保证后续可以回归。
但它并非官方 SDK，也不是已批准的正式 BrakeSync GPIO Driver 或 EAL 设计。后续讨论驱动模板时可以独立审查、保留或替换，不应未经评审复制到产品驱动中。

这里仅适用于无制动功率的开发板测试；是否实测通过以真实 Keil 编译、烧录和硬件证据为准。
