# 云途 YTM32B1LE0 Target 工程接入

**当前状态：WAITING_FOR_REAL_KEIL_PROJECT / TARGET_NOT_BUILT**。

本目录不是能够烧录的 Keil 项目。这里**没有**生成假的 `.uvprojx`、启动文件、链接脚本、SDK CAN API 或芯片引脚设置。

## 必须以实际本机已实测工程为起点

1. 从已成功 LED 编译/烧录/运行的 Keil 工程取得 `.uvprojx`、源文件清单、Startup、Scatter/链接配置、时钟和 PinMux 配置。
2. 核对实际芯片顶标、PCB Rev、YCT 与 SDK 完整版本、Arm 编译器、下载算法以及源文件许可证。
3. 供应商 SDK 不得未经许可证确认直接公开发布；对独立安装的 SDK 使用受控版本、目录/散列清单和可重复工程生成说明。
4. 将已核对的 CAN SDK 示例及实际回调/中断行为实现为 `can_bus_board.c`；适配前不能把 Host CAN 样板链接为真实 Target 驱动。
5. 构建后分别记录 Keil Build、Flash Verify、复位运行、真实 CAN 双节点收发和 Bus-off 试验；均不得互相替代。

## 明确禁止

- 用主机 `CMakeLists.txt` 或 Host 假板级函数充当 MCU 构建。
- 为通过编译而虚构 SDK 函数、IRQ 名称、GPIO PinMux 或寄存器。
- 连接真实拖挂制动器功率链，或者把逻辑 CAN/制动请求当作执行许可。

此阶段的验收：**真实 Target 文件及可复现构建后，再由负责人进行实板测试**。
