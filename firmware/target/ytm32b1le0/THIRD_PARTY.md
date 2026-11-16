# 官方工程来源及第三方代码声明

- 来源：用户提供 `Gpio_Interrupt_Demo.zip`（官方示例）；保留了示例的 CMSIS、YTMicro SDK 子集、启动、链接和生成配置。
- 原示例软件要求：SDK `YTM32B1LE0_1_4_0` 或更高、Keil MDK 5.36+；原 Keil 文件目标：`YTM32B1LE05`、`YTMicro.YTM32B1LE0_DFP.1.4.0`、ArmClang 6.24。
- 云途 SDK 文件大多标识 `SPDX-License-Identifier: BSD-3-Clause`；Arm CMSIS 标识 `SPDX-License-Identifier: Apache-2.0`。除本项目新增文件外，上游源码保持原始版权声明。
- `platform/devices/YTM32B1LE0/include/YTM32B1LE0.h` 包含 YTMicro 著作权声明，但文件内未发现 SPDX/单独授权文本。**这不构成对该文件公开再分发权的法律确认。** 仓库所有者需依据实际 SDK 软件授权条款进一步确认其传播范围。若限制公开，转为受控本地 SDK 依赖并记录重建方式。
- 第三方源码不是 BrakeSync 自研 ASW/BSW 的合规证明。原版 SDK 与后续修改应单独纳入版本、适用性及安全审查。
