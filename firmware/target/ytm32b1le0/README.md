# YTM32B1LE05 EVB — Keil

打开 `firmware/target/ytm32b1le0/KEIL/BrakeSync.uvprojx`。

Keil Group 直接对应 `app`、`asw`、`common`、`bsw/drivers`、`bsw/cdd`、`bsw/internal`、`bsw/board`、`bsw/mcal`、`bsw/services`、`startup`、`tests/target`。

本轮新加入：
- 已实现并参与 Keil 编译：`app_scheduler.c`、5 个 ASW .c、`drv_can_frame.c`、`drv_gpio_ytm32b1le0.c`；
- 只作为接口参与 Keil 目录浏览：CAN/UART/Timer/I²C/SPI 等 Driver、CDD、Port 头文件。没有硬件/协议实现，不可被调用或当作验证通过；
- `GPIO_IRQHandler` 归平台 GPIO 驱动拥有；EVB 自测使用 Driver API，功能须实板回归。

构建器设置、启动文件、厂商 SDK 位置、Scatter 和 Flash 参数未改。查看 Build Output，执行 Rebuild All，保存原始日志。CI 只检查 Host 和 Keil 路径/链接元数据，不能代表 Keil 实际编译/烧录。

当前仅 EVB_NO_BRAKE_POWER。
