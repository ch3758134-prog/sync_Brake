# FW-01｜云途 SDK/Target 构建就绪审计（管理研究记录）

> 记录日期：2026-11-13 · 状态：**源码检查已完成，Target Build 未执行、开发板未实测**。  
> 限制：本文件仅用于制定基于真实证据的开发任务；未改动 `firmware`、`app` 及外设硬件；不许可制动功率输出。

## 1. 审计输入与版本

| 来源 | 证据 | 能够证明的内容 |
|---|---|---|
| 用户历史工程 `BrakeSync_LE05_Bringup.zip` | SHA-256 `9333fce6d7655779449fe6661065fa3aca7ff3a07183b19670afd288c43e3fab` | 包含云途工程一部分 SDK/板级源码 |
| 用户历史工程 `BrakeSync_LE05_Bringup_FixThumb.zip` | SHA-256 `1385506babc94d354f3ba729c5eb507b2d394a916ce4b7ae6a67af9096acb15e` | 修订版 bring-up 工程；不是完整可再现官方 SDK 安装包 |
| 固件分支 HEAD | `d1f38b250f2c229aeb122985dc0520b969fe971d` | [GitHub 固件目录](https://github.com/ch3758134-prog/sync_Brake/tree/firmware/firmware) |
| 固件 Host CI | [run 43273051114](https://github.com/ch3758134-prog/sync_Brake/actions/runs/43273051114) | CMake/Host Build/Host Tests 成功；Target 未证明 |
| 当前硬件参考 | 已登记云途 RevC 原理图与 RevA 手册 | 针对实板版本的 PinMux / I²C 控制器差异仍须确认 |

## 2. 历史 SDK 工程源文件审查结论

**有直接源码依据：**

1. `CMakeLists.txt` 中 `SDK_VER 1_5_0`，编译目标定义 `YTM32B1LE0` / `CPU_YTM32B1LE0`；但 `SDK_DIR` 写死为旧电脑路径 `C:/Users/ming/AppData/Roaming/yt_config_tool/sdk/YTM32B1LE0_1_5_0`。这不能直接作为可重复 Target Build 基线。
2. `app/main.c` 只有 `Board_Init()` 和无限循环，且 `Board_Init()` 空实现。**不能把这一工程视作用户先前串口/CAN 功能实测的源代码证据**。
3. `board/pin_mux.c` 为空骨架，尚无已核对的业务引脚初始化。
4. `board/clock_config.c` 的外设时钟配置数组为空，`count=0`；仅配置了基础时钟结构，不能说明 UART/CAN/I²C 已得到外设时钟。
5. 归档内包含 `pins_driver.h/.c`、`clock_YTM32B1Lx.c`、`interrupt_manager.c`、CMSIS/设备启动与链接材料。**没有找到配套 UART、I²C、CAN、DMA 的完整驱动源码**，因此不能据此实现真实目标外设服务。
6. 包含 `yt_linker.ld`、Startup/CMake 相关文件，但本次不使用 ARM 交叉工具链构建，**没有新的 Target Build 证据**。

**对当前固件分支的影响：** `firmware/CMakeLists.txt` 主要是 Host 核心与单测目标，当前 `board/ytmicro/Le05GpioAdapter.c` 的 SDK 引用还没有得到本轮 Target 编译验证。现有 `BsPort` 回调边界可以继续作为可移植软件候选，但目标绑定必须以芯片/SDK 原件补全为前提。

## 3. 最小阻塞条件和可并行工作

| 任务 | 当前依据 | 继续前要确认的条件 | 允许开展的范围 |
|---|---|---|---|
| FW-TARGET-01 | 源码残片 + RevC 原理图 | 实板 PCB Revision、MCU 顶标、完整官方 SDK 版本/位置、工具链 | 完整目标构建配置、板级起动/时钟/GPIO，**不接输出功率** |
| FW-CAN-01 | BSW/CAN Host API 存在；EVB 有 CAN 收发器 | 已购加速度模块接口类型、供电、CAN 物理层、ID/波特率/帧协议 | 分离 CAN Driver 实现与传感器报文解析，可先做 Host 协议测试 |
| FW-BLE-01 | BSW UART API、WLT8016 透传层源码 | 实际蓝牙模块型号、固件、UART 电平/参数、GATT/AT 真实证据 | UART 异步收发基础、BLE 模块字节透传的安全隔离测试 |
| FW-MOTION-01 | `Bma510.c` 存在 Host 源码 | 实际传感器是否 BMA510 芯片板或 CAN 模块 | 上层运动数据接口、质量与超时逻辑可先 Host 测试 |

独立于实物的纯 C 输入有效性、状态、配置、参数边界和 Host 测试可以平行开展。**不得因模块型号未明就冻结其底层总线、引脚、报文/寄存器或制动算法标定**。

## 4. 下一份目标执行合同的必要输入（交给固件专业对话/Codex）

- **输入版本**：准确 `firmware` HEAD + SDK 完整版的版本/散列 + 实板 MCU 顶标。
- **限定文件**：`firmware/board/ytmicro/*`、批准的 Target 工程构建文件及相关测试；不跨入 `ASW/BrakeSystem` 实际输出实现。
- **限定动作**：先确认 SDK GPIO/时钟/启动符号和复用配置，独立通过 Target build，按实板给出 UART/CAN 引脚接线与验证步骤。
- **禁止事项**：不得猜 SDK API、将历史独立 Demo 覆盖正式源码、改 App 协议或引出真实制动功率。
- **应交付证据**：源码 diff、构建命令和编译器版本、ELF/Map、实板烧录和日志分开列示，未执行项目标 `NOT_RUN`。
- **触发条件**：完整 SDK 和实板身份明确后，由负责固件的研发对话形成正式实现/测试指令；管理 AI 不越权直接修改固件分支。

## 5. 本次研究结论

当前**不是零起点**：已有可审查的 Host C/BSW 骨架、部分官方 SDK 源码及开发板原理图；同时**还不具备声称 Target 完整集成已验证的证据**。与其在 `main()` 堆积演示代码，更合理的是保持现有 ASW/BSW 边界，明确固定 SDK 的 Board 层目标构建及 CAN/UART 真实模块测试合同。

**证据等级**：源码/资料审计完成；Target 编译、烧录、实际模块测试均为 `NOT_RUN`（本次工作）。
