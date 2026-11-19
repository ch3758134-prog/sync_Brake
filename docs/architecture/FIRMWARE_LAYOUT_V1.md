# BrakeSync 固件架构 V1.0

主线：**MCU SDK/外设 → EAL PCB/器件 → ASW 制动业务**。

- `firmware/source/main.c` 为唯一入口，旧演示 APP 迁入 `tests/target/evb_app.c/h`。
- `firmware/source/ASW` 按 BrakeInput、Motion、SystemState、Diagnostics、BrakeControl 分类；Algorithm 明确预留，真实算法未实现。
- `firmware/source/BSW/MCAL` 是跨平台外设合同，EAL 是 PCB 与器件抽象，Services 是时间/调度/缓冲/日志/健康机制。
- `firmware/platform/ytm32b1le0/sdk/` 的 69 个官方 SDK/CMSIS Blob 使用原 Git SHA 保留，YCT/Startup 文件也原样迁移。SDK **仅为 GPIO 示例带来的不完整子集**，不包含完整 CAN 控制器驱动。
- Keil Group 将自研、CMSIS、Yuntu SDK、YCT/启动和测试分开；不会因目录名称增加多余空壳驱动。

迁移前固件基线 `71a39c06b52af8a857834a6bdde25aec97afb226`。不改变 GPIO ISR、时钟、引脚及制动业务逻辑。Host CI 和 Keil 元数据需以迁移后提交重新验证；Keil ArmClang Rebuild、烧录、板级测试均 NOT_RUN，真实制动功率禁止。
