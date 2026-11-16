# BrakeSync firmware｜实施与验证状态

| 事项 | 当前状态 | 依据及说明 |
|---|---|---|
| 固件新基线 | 已建立 | `firmware` 分支，原旧代码仅保留 Git 历史 |
| 真实 Keil 工程文件 | 已纳入 GitHub | `target/ytm32b1le0/KEIL/BrakeSync.uvprojx`，89 个相对路径核对存在 |
| 官方 Demo 过去的 Keil Build | HISTORICAL_PASS | 用户所给原 Demo 的 2026-11-04 构建日志，ArmClang 6.24，0 errors/0 warnings；**非本次工程** |
| 新增 App/Board C 语法检查 | PASS_CLANG_SYNTAX | 本次在容器中使用 `clang --target=armv6m-none-eabi -fsyntax-only` 检查 3 个新 C 文件，非 Keil 构建 |
| Keil 新目标完整 Build | NOT_RUN | 当前没有 Keil ArmClang 6.24，本机由负责人执行；不可虚构 PASS |
| 设备 Pack / Flash 算法 | LOCAL_DEPENDENCY | 原文件要求 `YTMicro.YTM32B1LE0_DFP.1.4.0`，Flash `YTM32B1LE0_Main.FLM` |
| 实板下载 / Flash 校验 / 复位 | NOT_RUN | 负责人实测 |
| LED PTD7 心跳 / SW1 SW2 / LED PTD6 | NOT_RUN_NEW_TARGET | 功能代码已接入，需实物验证，不继承原 Demo 实测结论 |
| Host 单元测试 | 以 Actions 为准 | 使用之前的 GCC/Clang Host CI，不链接 Target MCU 源 |
| 实际云途 CAN / UART DMA 驱动 | NOT_IMPLEMENTED | 目标 SDK 子集包含 UART/DMA 基础源码，但 CAN 控制器驱动未包含 |
| MISRA C:2025 专业检查 | NOT_RUN | 静态分析和偏离报告待做 |
| 真实制动功率与车辆 | BLOCKED | 未完成硬件危险分析、实体保护及台架授权 |

现阶段**只用于 LED/按钮 GPIO 实板 bring-up 与后续外设集成**，不得连接电制动器。
