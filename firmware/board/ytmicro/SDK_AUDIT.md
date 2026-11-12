# 旧对话 SDK 核验记录（2026-11-12）

已从个人历史文件中找到**云途 SDK 部分源码**，不是仅有 PDF：

- `BrakeSync_LE05_Bringup.zip` SHA256 `9333fce6d7655779449fe6661065fa3aca7ff3a07183b19670afd288c43e3fab`
- `BrakeSync_LE05_Bringup_FixThumb.zip` SHA256 `1385506babc94d354f3ba729c5eb507b2d394a916ce4b7ae6a67af9096acb15e`

核实内容：
- CMake 工程 `BrakeSync_LE05_Bringup` 针对 `YTM32B1LE0`，`SDK_VER 1_5_0`。
- 原始 `SDK_DIR` 是旧本机路径 `C:/Users/ming/AppData/Roaming/yt_config_tool/sdk/YTM32B1LE0_1_5_0`，不是 CI 可复现路径。
- 已附带 SDK 源码：`pins_driver.h/.c`、`clock_YTM32B1Lx.c`、`interrupt_manager.c`、CMSIS、设备头文件、Board 时钟/中断/启动/linker。
- 提供且已核对的 GPIO API：`PINS_DRV_ReadPin(GPIO_Type*, uint32_t bit)` 和 `PINS_DRV_WritePin(GPIO_Type*, uint32_t bit, uint8_t value)`。
- 对应压缩包**不包含**完整 UART、I2C、CAN、定时器、DMA 外设驱动源码。不能借此认定这些芯片适配已完成。
- `board/pin_mux.c` 尚无业务引脚配置；`board/clock_config.c` 尚无目标 UART/CAN/I2C 时钟绑定。
- FixThumb 压缩包是一个修订版 bringup 工程，不等于经过系统级验证的 SDK 发布包。

因此：
1. `Le05GpioAdapter.c` 是首个**实际引用云途 PINS_DRV API** 的目标平台适配模块，尚未完成 Target Build。
2. 不把第三方 SDK 原文直接复制到公共 GitHub；用户本地需要按已授权 SDK 位置显式接入。
3. 后续从 **YTM32B1LE0 官方 SDK 完整包**核对 UART/CAN/Timer/I2C/DMA 模块后，再实现 Target Driver 并建立单独的构建门禁。
4. 不应把先前 `Project.zip`（使用 `fsl_*` 的其他平台工程）当云途 SDK。
