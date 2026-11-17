# 云途 SDK / CMSIS 来源

来源：用户提供 `Gpio_Interrupt_Demo.zip`，原官方示例中的 YTMicro SDK **子集**、CMSIS、OSIF、YCT 配置、板级启动链接文件。

- 原 Demo 所用目标是 YTM32B1LE05，Keil 历史记录 ArmClang 6.24、YTMicro.YTM32B1LE0_DFP.1.4.0；不能推出完整 SDK 版本包含全部外设。
- 厂商 MCU SDK 原始文件现位于 `firmware/bsw/mcal/ytm32b1le0/sdk`；搬迁保留 Git blob SHA，便于来源和版本溯源。
- YCT 生成配置和板级启动链接文件位于 `firmware/bsw/board/ytm32b1le0/generated`。
- 现有来源声明包括 BSD-3-Clause 与 CMSIS Apache-2.0。个别 MCU 设备头文件无明确再分发条款：仓库公开发布的授权仍需由所有者核对正式 SDK 许可证，不可根据文件存在默认已获许可。
- SDK 未经功能安全工具资格化、独立审查或产品级实际测试；本项目自研层也尚未通过相应评定。
