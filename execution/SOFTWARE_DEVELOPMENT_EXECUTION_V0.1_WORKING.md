# BrakeSync｜嵌入式软件工程开发路线与驱动实施任务 V0.1

**日期：** 2026-11-14  
**状态：** `SOFTWARE_EXECUTION_PLAN_WORKING`（基于已批准 P0–P8 V0.3 的软件落地细化，非新的 P0–P8 审批）  
**本轮实际产物：** 全新固件工程模板 `BrakeSync_全新固件工程模板_V0.1.zip`，已在当前执行环境完成 GCC 14.2 + CMake 3.31 + Ninja 的 Host Build 和 1 项 CTest 测试套件；没有完成 ARM Target Build/烧录/外设实测。  
**开发板：** YTM32B1LE0-EVB-LQFP64，实板 PCB Rev 和精确 MCU 顶标尚需确认。正式产品 PCB 未完成。  
**产品顺序：** V1 真实功能测试 → V2 LCD 完整交付 → V3 手机 App。主控制软件始终尽量共用。  
**协作：** ChatGPT 负责研究/设计/实施任务；Codex 在明确任务范围后新建/修改代码与执行构建；负责人核实实物、完成烧录测量及验收。  

---

## 1. 纠正工程推进方向

前面的 P0–P8 文档已经确立系统功能和安全约束，但没有把嵌入式开发切成能逐项实施的工作。现在要把**系统框架**与**软件工程框架、驱动功能测试**同步推进，而不是等正式 PCB 完成再启动驱动。当前先完成环境、全新工程、板级基础驱动、真实传感器/蓝牙通信、无功率控制联调。

**不采用的方式：** ①复用历史 `firmware/app` 实现作为新基线；②把所有初始化/ISR/DMA代码放 `main.c`；③没有真实 SDK 依据就写“已适配云途”驱动；④先把大量通用 HAL/OS 框架搭起来再找使用场景；⑤将 Host PASS、Target PASS、烧录 PASS 和真实制动输出混为一谈。

## 2. 开发软件环境和硬件准备（E00）

### 2.1 推荐主工具链

| 工具 | 目的 | 当前结论 |
|---|---|---|
| Windows 工作站 + VS Code（C/C++、CMake Tools） | 编写/调试固件；读取生成工程 | **建议主路线**，与历史 YCT/CMake 工程形式相容；实际安装待负责人核验 |
| 云途 YT Config Tool（YCT） | 选择芯片/官方 SDK，生成时钟、引脚、外设及可编译工程 | 云途官方确认支持 YTM32B1LE0，需核对具体版本、PCN 补丁和 SDK |
| YTM32B1LE0 官方**完整** SDK | MCU 外设驱动及示例 | 当前只有历史 bring-up 的部分 SDK 文件，缺完整 UART/CAN/DMA 支撑，不得代替完整安装包 |
| Arm GNU Toolchain (`arm-none-eabi-gcc`)、CMake、Ninja | ARM Target 编译 + 可复现构建 | 此环境有 Host gcc/CMake/Ninja，**未检测到 `arm-none-eabi-gcc`**，Target 未验证 |
| J-Link / 目标支持的调试器、J-Flash/Ozone/GDB Server | 下载、复位、单步、断点和变量查看 | 型号、固件及目标 MCU 支持待核实 |
| USB-UART 串口终端、逻辑分析仪、示波器 | UART/DMA/IRQ/时间基准的波形和日志 | 按实物使用 |
| USB-CAN 或第二 CAN 节点及正确端接 | CAN 发送/接收与错误恢复 | 按实际加速度模块协议核对 |
| Git | 分支/版本/构建证据 | 代码尚未写入 GitHub 的 `firmware` 分支，当前管理分支只归档方案 |

官方资料入口：

- 云途 YCT 介绍（支持 LE0、可生成 Keil/IAR/GCC+CMake）：<https://www.ytmicro.com/news-details-37.php>
- 云途工具链/SDK 介绍：<https://www.ytmicro.com/product-5.php>
- 云途论坛：LE0 手动搭建工程/SDK 获取途径：<https://forum.ytmicro.com/topic/1632/ytm32手动搭建环境>

**启动前清点：** ①EVB 丝印 PCB Rev、MCU 顶标和已接的下载接口；②与实物 Revision 适用的原理图；③YCT 版本、完整 SDK 包名与 SHA-256/来源；④GCC/调试器版本；⑤可用串口或 LED 的真实连接；⑥蓝牙模块和加速度模块的型号、供电、接口、实物接线；⑦独立电源/隔离测试工具。实际使用的 RevA 板手册与 RevC 图纸存在 I²C0/I²C1 冲突，不能在未核实时直接从任何一份复制 PinMux。

### 2.2 工具链验证门槛

在 Windows PowerShell 中依次检查：

```powershell
where.exe git
where.exe cmake
where.exe ninja
where.exe arm-none-eabi-gcc
arm-none-eabi-gcc --version
```

然后在 YCT **新建** MCU 精确型号对应工程，固定 SDK/生成配置、编译出 ELF、通过调试器下载并能断点复位；此项才算 `E00/E01 TARGET_TOOLCHAIN_PROVEN`。不能仅凭工具在 PATH 就标记完成。

## 3. 新的工程模板和最终工程布局（E01/E02）

本轮已实际建立可重复 Host 构建的**全新骨架**。它绝不是“已经写完的云途固件”：不包含未核对的 MCU 头文件、SDK 驱动、目标链接脚本和真实执行输出。

```text
BrakeSync_Firmware/
├── CMakeLists.txt              # 目前仅 Host 构建，已实测
├── include/brakesync/
│   └── control_core.h          # 共用业务纯逻辑入口
├── src/
│   └── asw/control_core.c      # 不引用芯片 SDK；当前只观测输入
├── tests/test_control_core.c  # Host 单元测试，实际已跑
├── examples/host_smoke.c      # 仅 Host 的模拟入口
├── target/ytm32b1le0/
│   └── README.md               # 新建 YCT Target 工程及接入步骤
└── docs/
    ├── ENVIRONMENT.md
    ├── DRIVER_TASKS.md
    └── TEST_EVIDENCE.md
```

Target 工程通过 YCT 重新生成、首次烧录通过后，逐步形成以下模块布局（**以下目录是待实施目标，不意味着已经存在源码**）：

```text
firmware/
├── app/
│   ├── app_main.c             # 调度业务入口；不放外设和 ISR 细节
│   └── app_scheduler.c        # 按需使用的周期轮询调度
├── asw/
│   ├── input_quality/         # 制动输入、运动信息有效性/新鲜度
│   ├── brake_control/         # 同步制动控制逻辑和受控请求
│   ├── system_state/          # INIT/READY/ACTIVE/FAULT 等业务状态
│   ├── configuration/         # 重量档/制动力、校验/版本/保存
│   └── diagnostics/           # 故障、记录和恢复语义
├── bsw/
│   ├── drivers/               # GPIO、UART DMA、CAN、时间基准等驱动封装
│   ├── devices/               # CAN 加速度模块/蓝牙真实设备协议
│   ├── services/              # 必要的时间、NVM、事件队列等服务
│   └── board/                 # 评估板 PinMux、时钟、SDK glue
├── target/ytm32b1le0/         # YCT 生成工程/目标构建
├── tests/host/                # 控制和协议解析的 Host 单元测试
├── tests/target/              # 不带实际制动功率的板上测试入口
└── docs/                      # 环境/接口/模块验证/软件决策
```

**抽象的适度原则：** ASW 不引用 SDK；目标 MCU 差异只在 `bsw/board`、必要的 `bsw/drivers`；复用 SDK 已有的驱动而不是再套若干没有价值的 MCAL 层。模块驱动处理具体中断、DMA 状态与错误，`main` 只负责 `Board_Init`、`App_Init`、`App_RunOnce` 一类调用；这些 Target API **尚未实施**。

**调度建议：** V1 先使用简单的裸机主循环 + 单调时钟 + 事件标志/队列；ISR 负责最少的数据收集与上报，业务处理在主任务完成。先量测周期/CPU/ISR 占用，再确定是否需要 RTOS，不强制引入操作系统。

## 4. 驱动先行：把每一步拆到编写和实测文件

| 先后 | 任务 ID | 这一步实际要建的模块/代码 | 首次上板的可重复验证 | 重要异常场景/限制 |
|---|---|---|---|---|
| 0 | SW-00 | `docs/ENV.md`、YCT 配置快照、工具链/SDK 版本记录 | 同一份 YCT 生成工程能清理后重建 | MCU 顶标/SDK Rev 不符，必须阻塞 |
| 1 | SW-01 | 全新 YCT 工程、Target CMake、启动和链接，最小 `main` | 编译 ELF/HEX、烧录、复位和断点；不写业务 | 不导入旧工程，不修改官方 SDK 源 |
| 2 | SW-02 | `bsw/board/clock`、复位原因、启动资格 | 通过工具/测量核对真实时钟和重启 | 振荡器故障/切时钟错误，不编造频率 |
| 3 | SW-03 | `bsw/drivers/gpio` + `bsw/board/pinmap` | LED/测试针脚输出，外部电平输入、GPIO IRQ | 上电默认状态、拉电阻、未连接引脚，不触及功率端 |
| 4 | SW-04 | `bsw/services/timebase` + 定时器中断 | 毫秒时间戳、节拍、超时测量、定时抖动/溢出 | 禁止在控制循环使用长时阻塞延时 |
| 5 | SW-05 | `bsw/drivers/uart_dma` | DMA 发送、DMA 接收、缓冲区分帧、IRQ/回调、连续数据测试 | DMA 超限、断帧、复位后重启、RX 缓冲溢出 |
| 6 | SW-06 | `bsw/drivers/can` | 第二 CAN 节点实帧收发、滤波、时间戳、错误计数 | 丢帧、错误帧、Bus-off 及恢复；不猜 CAN 波特率 |
| 7 | SW-07 | `bsw/devices/imu_can` **如真实加速度模块是 CAN** | 查模块手册得到 ID/数据单位与变化规律；长时间帧流与超时 | 不能用 BMA510 I²C 手册推断 CAN 模块协议 |
| 8 | SW-08 | `bsw/devices/ble_uart` 与需要的串口解析器 | 核对实物模块 AT/数据模式、上电/连接/断链回执 | AT 与透传模式不能混淆；超时/重入/非法包 |
| 9 | SW-09 | `bsw/drivers/i2c_or_spi` + `bsw/devices/bma510` **仅当实物身份对应** | 真实 BMA510 寄存器读写、采样速率、方向与传感质量 | RevA/RevC I²C0/I²C1 冲突需先消项 |
| 10 | SW-10 | `bsw/services/nvm`、`diag`、`watchdog` | 数据保存/读回/错误/复位后恢复，诊断自检 | 掉电、CRC 错、写中断、复位和喂狗不合格 |
| 11 | SW-11 | `asw/system_state`、`input_quality`、`configuration` | Host 用例 + 无功率 Target 状态和参数整合 | 失效/过期/输入冲突、非法配置、错误状态显示 |
| 12 | SW-12 | `asw/brake_control`（先输入快照与逻辑请求） | 逻辑算法 Host 测试、目标运行的周期/时限日志 | 制动力映射与故障动作不得凭空选数 |
| 13 | SW-13 | `tests/target/no_power_integration` | 刹车输入→运动数据→业务状态→逻辑请求，全链路无功率验证 | 物理不连接房车制动功率，记录原始输入/结果 |
| 14 | SW-14 | 功率与制动器受控台架（**后续单独工作包**） | 只有通过专用功率硬件、设计审查和台架安全许可后才允许 | 此阶段当前 `BLOCKED`，不让 MCU 开发板直接驱动轮端 |

### 4.1 GPIO 驱动不是“闪个灯就结束”

最少应包括初始化与 PinMux 来源、输入/输出、拉电阻/中断配置、状态读写接口、非法参数处理和复位默认态的检查。测试结果用示波器/万用表/逻辑分析仪记录。**仅 LED 闪烁属于入门 smoke test**。

### 4.2 UART DMA 开发的正确顺序

1. 从 SDK 确认该 UART 的 DMA request、TX/RX channel、IRQ、空闲帧或超时机制，不直接照抄其他 MCU 的寄存器。
2. 先验证单方向 DMA TX，ISR/回调通知传输完成；正常应用调用层**不走阻塞发送**。
3. 验证 RX DMA（循环/双缓冲/分段，依 MCU/SDK 能力选），处理连续流、半包/多包、超时和溢出。
4. `uart_dma.c` 负责启动、停止、缓冲状态和错误；`uart_dma_irq.c`（需要时）负责中断适配；`ble_uart.c` 才处理模块协议。**中断回调和数据解析不能堆到 main。**
5. 记录串口真实吞吐率、最坏延迟、空闲/持续流及错误恢复。没有这些不能标记 `UART_DMA_TEST_PASS`。

### 4.3 CAN 驱动开发的正确顺序

1. 核实评估板 CAN 收发器、STB/EN 与实际硬件连线、引脚复用、时钟与收发器供电。
2. 先与第二节点完成指定 CAN 帧收发，再测试滤波、中断接收、TX 完成和错误/Bus-off 恢复。
3. 在设备层解码真实 CAN 加速度模块的报文 ID、缩放、字节序、方向、更新周期和失效规则，不把协议解析写进 CAN 控制器驱动。
4. 分开保留“CAN 只是加速度模块通信”和“牵引车 CAN 车辆信息输入”的产品决策，前者不自动意味着控制器必须依赖牵引车 CAN。

### 4.4 蓝牙和加速度设备的驱动边界

- **蓝牙**：底层 UART DMA 驱动与模块协议/状态机分开；V1 可做蓝牙真实模块连通及数据通路实验，但这不是 V3 正式 App 产品实施。
- **加速度**：先看实物铭牌和手册决定是 CAN 加速度设备还是 BMA510 芯片 I²C/SPI 模块，分开写驱动；不能因历史资料同时有 BMA510 PDF 与“CAN 加速度计”就当作同一个器件。

## 5. 软件架构的依赖与配置规则

```text
V1/V2/V3 的上层业务 (ASW: input_quality / brake_control / state / config / diag)
                     ↓ 使用受控、窄接口
基础服务 (时间 / 事件 / 参数存储 / 数据质量)
                     ↓
设备驱动 (CAN加速度模块 / BLE模块 / 未来LCD驱动)
                     ↓
MCU 适配/驱动 (UART DMA / CAN / GPIO / Timer / I2C / NVM)
                     ↓
官方 YTM32 SDK + MCU + EVB PinMux/Clock/IRQ 配置
```

**特别约束：** ASW 不能直接操作 MCU 寄存器；但也不需要把官方 SDK 的每个 API 原样包三层。按照硬件变更会影响哪里、Host 测试能否替换驱动、功能职责是否独立，决定抽象程度。使用统一 C 命名、简洁中文注释、静态内存、无动态堆、明确错误码/超时/恢复条件。

## 6. 与三版产品的任务关系

| 软件能力 | V1 真实测试版 | V2 LCD 交付版 | V3 App 智能版 |
|---|---|---|---|
| 同步控制核心、运行状态、输入质量和诊断 | **主研发** | 继承并回归 | 继承并回归 |
| MCU 驱动、传感器、通信、NVM、时间、看门狗 | **优先完成必要部分** | 根据 LCD 产品扩展 | 根据 App/无线扩展 |
| LCD 驱动与实体按键/HMI | 不要求完整产品界面，可保留基础调试 | **主研发与交付** | 按产品配置复用 |
| BLE UART 模块 | 有实物可先做独立功能测试 | 不设为首个正式产品的必备条件 | **实际设备/App 协议联调** |
| App 代码 | 不作为 V1 的完成条件 | 不阻塞 LCD 交付 | **主研发** |
| 功率安全、真实制动器执行与台架验证 | **必须等实体安全准入后逐级完成 V1 目标** | 在 V1 验证结果上充分回归 | 更改关键路径后充分回归 |

## 7. 验收标准和禁止的假进度

每个开发任务单独标注：

- `DESIGN_READY`：接口和异常场景已经审查。
- `HOST_BUILD_PASS`、`HOST_TEST_PASS`：Host 实际编译/测试且有日志。
- `TARGET_BUILD_PASS`：目标 ARM 编译/链接完成（版本和产物明确）。
- `FLASH_PASS`：真实板卡下载成功。
- `TARGET_RUNTIME_PASS`：目标板运行/断点/日志正常。
- `MODULE_HW_PASS`：外设或模块有输入/输出波形、命令和实物证据。
- `INTEGRATION_PASS`：规定范围内真实模块联调通过。
- `POWER_BENCH_PASS` / `VEHICLE_PASS`：具备独立许可的功率/车辆试验后才能使用。

**当前事实**：本轮全新固件骨架在当前 Linux 环境使用 GCC 14.2、CMake 3.31、Ninja 完成 Host 编译，CTest 1/1 测试套件通过。实际物理制动许可始终为禁用。**未完成 ARM Target 编译、真实开发板烧录、UART DMA、CAN、BLE、加速度模块实测，未实施制动控制律和执行输出。**

## 8. Codex / ChatGPT / 项目负责人协作和仓库实施

| 分工 | 工作责任 |
|---|---|
| ChatGPT | 确定任务的接口职责、软硬件依据、实现边界、异常场景与验收条件；审查代码/测试输出 |
| Codex | **仅按单个任务包**新建/修改文件、运行 Host/Target 可用工具、提交精确结果；不得自主改变控制架构/功率安全策略 |
| 负责人 | 确认实板版本、接线及测试条件，执行必要烧录/测量，审核真实结果及产品安全决策 |

GitHub 处理建议：维持独立 `management`、`firmware`、`app` 分支，按负责人原有分支规则，不自行合并 `main`。本轮源代码模板**只在当前对话交付**，并未写入 `firmware` 分支。未来开始新工程实施时由负责人决定在 `firmware` 分支创建全新代码基线；不要直接复制或沿用旧代码历史中的实现结论。

每个 Codex 任务必须含：任务 ID、唯一可修改的文件/目录、实际 SDK/原理图版本、预期接口行为、禁止事项、编译/测试命令、验收证据、失败报告与提交要求。

## 9. 下一步立即执行的三个工作包

| 任务 | 现在要做什么 | 交付完成标准 | 当前状态 |
|---|---|---|---|
| **SW-00** | 把 EVB Rev/MCU 顶标、官方 SDK/YCT、GCC 和调试器版本登记，确认原理图匹配 | 完整环境表、板卡与 SDK 匹配结论 | `EVIDENCE_OPEN` |
| **SW-01** | 用 YCT 新建目标 MCU 空工程（不复制旧 Bringup），保留生成配置；完成最小 `main`、ELF/HEX Target 编译、烧录与断点 | 真正的 Target Build / Flash / Runtime 分项记录 | `NOT_RUN` |
| **SW-02** | 在新工程上先做时钟→GPIO→时间基准，随后 UART DMA→CAN | 每个驱动独立文件 + IRQ + 正常/错误实际测试，日志/波形可审阅 | `NOT_RUN` |

**本轮已有可用起点**：`BrakeSync_全新固件工程模板_V0.1.zip`，里面的 Host C11 工程已经实测。它的 `target/ytm32b1le0/README.md` 指明如何接入新的 YCT Target 工程；未取得对应完整 SDK 和实物依据前，不生成假驱动源码。

**结论：** 从现在开始以“环境、工程、驱动、设备、业务、联调”的顺序推进软件，不再先做额外一轮 P 阶段审批。产品 PCB 可以后做，但真实制动器功率输出必须独立安全准入。
