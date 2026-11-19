# BrakeSync Firmware

`source/main.c` 启动和 EVB 测试；`source/ASW` 为业务（Algorithm 未实现）；`source/BSW/{MCAL,EAL,Services}` 是外设合同、PCB/设备抽象和基础服务。`platform/ytm32b1le0` 将官方 SDK/CMSIS、YCT、启动、自研 Port 分开。`tests/{host,target}` 长期保存测试；`target/ytm32b1le0/KEIL` 为 Keil 工程。

详细见 [架构文档](../docs/architecture/FIRMWARE_LAYOUT_V1.md)。仅 EVB_NO_BRAKE_POWER；目标 Keil Build、烧录与实板待验证。
