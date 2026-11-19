# BrakeSync 工程模板基线 V1.0 — 验证边界

此文档记录模板构成与验收项目，不把 GitHub CI 结果等同于开发板实测。

| 项目 | 检查方式 | 当前含义 |
|---|---|---|
| MCU Platform | 查看 `platform/ytm32b1le0` | SDK、YCT、Startup、System Init 明确分离 |
| SDK 原始内容 | `firmware/tools/check_platform_origin.py` | 保护 89 个官方示例源 Blob；不检查芯片功能正确性 |
| Keil 结构 | `firmware/tools/check_target_project.py` | 检查工程引用、头文件目录、Scatter；不执行 ArmClang |
| Host C / 单元测试 | GitHub `firmware-host`（GCC、Clang） | 验证可移植模块和模拟测试；不运行真实硬件 |
| Target 编译 | Windows Keil Rebuild All | **NOT_RUN**；需负责人实际检查 |
| 下载与运行 | 调试器下载、单步、运行 | **NOT_RUN** |
| GPIO / Timebase | `tests/target` 既有函数 | **NOT_RUN**；旧 GPIO 实验代码并未认定为产品驱动 |
| CAN/UART DMA/BLE/IMU | 后续 Driver 与设备协议开发 | **NOT_IMPLEMENTED 或仅候审接口** |
| 产品制动算法/功率 | 安全约束、需求、算法、真实 PCB | **未实现并且真实功率禁止** |

目录模板不构成车辆使用许可。固件依旧为 **EVB_NO_BRAKE_POWER**。
