# MCAL — 设计阶段的 MCU 外设抽象边界

这个目录为未来跨 MCU 外设能力保留位置，但**目前并未批准统一 Driver 模板**。

现有 `drv_can.h`、`drv_uart.h`、`drv_timer.h` 等属于之前阶段的候审接口，需要后续结合真正的云途完整 SDK、芯片资料、DMA/IRQ 机制及其他 MCU 的实际差异逐项审查，不可将“接口头文件存在”当作“驱动完成”。

当前 `drv_can_frame.c` 仅为 CAN 帧格式校验，不实现控制器收发。这里不复制厂商 SDK；是否直接调用 SDK 或写最小适配，留到驱动设计讨论确定。

开发板 GPIO 的旧逻辑 ID 和旧实验驱动暂放在 `tests/target/legacy/`，并不在此宣布为正式 MCAL。
