# MCU 平台 — YTM32B1LE0

这个目录只管理**如何使 MCU 按已知配置启动并运行**，不是另一套 GPIO/CAN/UART Driver 框架。

| 路径 | 来源 | 作用 |
|---|---|---|
| `sdk/` | 芯片厂商原示例随附 | Device、CMSIS、PINS、Clock、UART、DMA、OSIF 等**有限子集** |
| `startup/` | 原示例 | Reset、向量、RAM 初始化和 Scatter |
| `config/` | YCT 工具生成 | Clock/PinMux/IRQ/DMA/UART 配置及原 `.yct` |
| `system_init.c/h` | BrakeSync 自研 | 按 YCT 配置调用 `CLOCK_SYS_Init/UpdateConfiguration` 与 `PINS_DRV_Init` |
| `system_time.c` | BrakeSync 自研旧模块 | 为现有时间服务调用 `OSIF_GetMilliseconds`，后续时基设计讨论时审查 |

原 `board_support.c/h` 已重命名为 `system_init.c/h`，没有新增硬件时钟/引脚配置。
原 `mcal_time.c` 只改存放位置，不代表它已经成为批准的通用 Timer Driver。

**这份 SDK 不是完整云途软件包。** 当前不能声称已具备完整 CAN、Flash、Watchdog 等硬件驱动。
不得修改官方 SDK 文件以掩盖缺失；版本及再分发许可另行核实。
SDK/YCT/Startup 原始 Blob 由 `ORIGIN_GIT_BLOBS.tsv` 和 CI 保护。

开发板验证时严禁接入实际拖挂制动功率。
