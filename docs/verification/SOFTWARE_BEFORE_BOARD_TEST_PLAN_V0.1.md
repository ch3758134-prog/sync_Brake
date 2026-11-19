# BrakeSync｜先完成软件、后集中逐驱动实测的实施与验证方案 V0.1

**状态**：IMPLEMENTATION_PLAN_WORKING（待负责人审阅；不是新的软件接口批准或实测声明）  
**基准**：`firmware@0a825a2`、管理分支 P3 SF-01～09、P4 DD-01～08 / SW-ICD-01～05、BS-SW-STD-001 V1.0。  
**范围**：本文件规划嵌入式 `firmware`；手机 App 在独立 `app` 分支研发，接口通过 HMI 合同协调；不自动扩大 V1 到 V2 LCD 或 V3 App 产品验收。  
**安全边界**：当前全程 EVB_NO_BRAKE_POWER；未批准的功率执行、轮端制动器与车辆测试始终禁止。  

## 1. 根本管理规则

1. **开发 ≠ 实测**：软件实现、Host 构建/单测、Target 编译、烧录、实板工作与产品级验收分别记证据。用户不必每次代码提交后执行实测。
2. **先完成可离线开发部分**：优先落实公共驱动与 Port 的 C 合同、静态资源、任务调度、状态/故障/参数、数据质量、协议编码与解析、Host 适配和单元测试。真实硬件 Port 仅依据已核验的 MCU SDK/官方示例实现，不创造不存在的寄存器/API。
3. **不能伪造软件“全部完成”**：产品算法标定、制动意图物理来源、CAN 传感器真实协议、LCD 选型、最终 PCB、功率安全故障动作，缺资料时定义数据/边界和缺口，不填写虚构常数、不返回虚假的成功。
4. **先集成再安排实测**：Keil .uvprojx 引用、IncludePath、Target 参与状态与 CMake 同步检查必须随代码提交；不把 IDE 整理工作交给用户。Host 通过 ≠ ArmClang 实际通过。
5. **一套源码、分开构建**：同源 ASW/业务 CDD/可移植 Driver Core；Host 使用测试 Port；目标固件使用唯一 MCU Port。所有 TEST/Host Fake 禁止误入正常 MCU 目标。
6. **测试资源统一安排**：先交付锁定 SHA 的无功率测试版、逐项操作手册、日志/异常用例和证据记录；用户自主决定测试日程。独立任务可以继续开发，但未测的特定硬件条件不予放行。

## 2. 编码工作包（此表为先后依赖，不是要求用户逐包上板）

| 工作包 | 源码范围（维持现有目录） | 软件交付与可离线验证 | 硬件依赖处理 |
|---|---|---|---|
| F00 基线收口 | common、docs、config、CMake、Keil、tools | 统一状态码/时间与数据质量类型；解决 V0.2 候审 API 冲突；编译成员清单与源文件覆盖门禁；Host/Target 目标配置独立 | Keil ArmClang 真正 Build 暂记 NOT_RUN，直到可以执行 |
| F01 运行平台 | app、bsw/services | 系统初始化/错误路径、周期+事件调度、超期/丢事件/时间回绕、静态队列、系统健康与故障日志 | 无准入周期值不得冒充正式控制周期 |
| F02 通用 Driver | bsw/drivers、bsw/internal | GPIO/Clock/Reset/Timer/UART/CAN/I2C/SPI/ADC 的公共 API 与能力查询；确有复用价值的异步事务、事件、缓冲、状态 Core；Host Fake/合同单测 | 定时器精度、DMA RX、CAN 实际邮箱/Bus-off 行为留待目标版本核验 |
| F03 MCU Port/Board | bsw/mcal、bsw/board、startup | 保留官方 SDK 和自然物理目录；基于已具证据的引脚、Clock、Timer、GPIO、UART DMA、CAN、I2C/SPI 等逐项适配；不满足条件的模块构建期禁用并标明原因 | 对缺失的完整 SDK/原理图/真实模块能力明确 BLOCKED_HW_EVIDENCE；不写假成功驱动 |
| F04 CDD 设备 | bsw/cdd | CAN IMU 周期命令与数据新鲜度、BMA510 采样/单位/方向、WLT8016 BLE 命令与连接状态；协议解析用测试向量验证 | 仅依据确切手册版本填 CAN ID/周期、BLE 命令、传感器寄存器；未确认项不冒认 |
| F05 ASW 核心 | asw | brake_intent、motion_signal、run_qualification、system_state、diagnostics、configuration（接收/校验/生效/持久/读回证据）、HMI 合同、设备身份、control_core 输入快照与逻辑请求 | 缺车辆制动律/标定时可实现状态和拒绝路径，不得构造真实刹车幅值或准入 |
| F06 集成与无功率测试 | app、tests/host、tests/target、qa、target | 建立无功率集成、统一日志/故障注入/数据可观测点、逐驱动测试入口、资源和最坏周期预算记录、构建与测试说明；冻结验收 SHA | 实板未执行时全部对应 HW/RUNTIME 状态 NOT_RUN |

**交付完整性约束**：上述 F00～F06 每个工作包必须有模块实现清单（IMPLEMENTED / PORT_BLOCKED / DESIGN_ONLY / NOT_APPLICABLE）、Host 用例、目标工程构建状态和未决依据；不以新增几十个头文件冒充可运行源码。

## 3. 双目标构建和 Keil 防遗漏规则

- 正式源文件清单由工具按实际 `*.c` 与受控排除名单生成或校验，不靠硬编码项目文件总数。
- 正式 MCU Target 编译源含 MCU 适配与所启用业务 Core；Host 目标使用自己的 Port Fake；不能把 Vendor SDK 放进 Host 编译。
- 新增 `.c/.h` 必须在同一变更中说明：Target/Host/both/仅设计，若应参与 Keil，必须更新 `.uvprojx`；若禁用则有明确状态和理由。
- CI 至少检查：公共 C 编译与单测（GCC、Clang）、头文件接口契约、Keil 路径/分组/启用源覆盖、Host 假实现未进入 MCU Target、无功率输出编译配置限制。
- Keil 实际 ArmClang 编译只能在可用工具环境内执行并归档结果；没有环境不声称 Build PASS。
- `main.c` 仅启动与调用 APP；ISR 在各驱动 Port 唯一实现；测试代码不再长期直接操作寄存器或持有 IRQ 向量。
- 版本锁定：`Board`、`Variant`、`Release` 独立管理；`firmware` 与 `app` 不自动合并 `main`。

## 4. 用户的实测批次：软件准备好后逐项进行

| 编号 | 验证项 | 先决条件 / 主要证据 | 无功率安全与异常路径 |
|---|---|---|---|
| T00 | 锁定固件、Keil Rebuild / 下载 | 锁定提交 SHA、Build Output、Flash Verify、复位运行 | 完整无功率版，不接功率回路 |
| T01 | Clock / Reset | 核对时钟源、核心/总线频率、复位标志、错误启动路径 | 时钟失败不应进入正常控制资格 |
| T02 | GPIO / EXTI | 已核对 EVB 引脚；读写、边沿 IRQ、输入上下拉及事件计数 | 不用未知引脚作输出；验证上电默认态 |
| T03 | Timebase / Timer / Scheduler | 周期精度、抖动、计数回绕、任务超期和恢复 | 不以 LED 观察代替量化时间验证 |
| T04 | UART / DMA | USB-UART/示波器；TX、RX、连续流、溢出、中断事件、异常恢复 | DMA 通道/请求源需与真实配置一致 |
| T05 | CAN Controller | 第二个 CAN 节点、收发器/端接；滤波、TX 完成、RX 时间戳、bus-off | 不根据驱动入队成功声称节点收到 |
| T06 | CAN IMU | 真实模块协议已核对；命令周期/状态响应/采样/丢帧/断电重连 | 设备采样时间与 MCU 接收时间分开 |
| T07 | I2C/SPI 与 BMA510 | 总线抓取、器件身份读取、读写寄存器、方向/异常 | 先确认是实际 BMA510，避免以 CAN 模块代替 |
| T08 | BLE / WLT8016 | 正确模块型号、UART 通道；AT/透传、连接/断链、指令/状态 | 手机失联不改变本地实时控制 |
| T09 | NVM / WDG / ADC（如有依据） | 参数掉电恢复、故障复位、ADC 标定、异常注入 | 看门狗触发必须可控；无真实功率测试 |
| T10 | 无功率控制链集成 | 真实/回放输入 → 质量 → 状态 → 逻辑请求 → 观测日志 | 不连接执行器；不得把逻辑请求当物理制动 |

**安排方式**：T00 一次准备；之后按 T01～T10 独立执行与记录。存在实际依赖（例如 Timer 前须确认 Clock，CAN IMU 前须确认 CAN）时才阻止对应子项；无关的测试不因其它硬件问题被迫停工。

## 5. 软件交付门槛与用户实测交接

### 软件包准备好、允许开始集中板测（TEST_READY）

- 可离线实现的核心模块具有源文件、使用说明、反向/边界用例与实际 Host 结果。
- Keil 所有应编译的真实源文件均已登记，IDE Group/IncludePath 完整；保留 SDK 和 Startup 原样。
- 各驱动提供实际测试入口和可观察结果，不把全部测试塞进 `main`；功能测试模式不得开启任何真实制动功率。
- 每个设备的实物依赖列出：准确型号、官方 SDK 版本、协议、接线、波特率、工具和安全条件。缺资料项标 BLOCKED，不代填。
- 交付一份固定提交 SHA、烧录/复位说明、逐项测试步骤、预期行为、异常场景、记录表和故障复现/回滚说明。
- 若真实 ArmClang 构建尚无法执行，标 `TARGET_BUILD_NOT_RUN` 并作为 T00 首项；不可把 TEST_READY 解读为已能烧录。

### 独立的测试证据状态

`HOST_BUILD`、`HOST_TEST`、`TARGET_BUILD`、`FLASH`、`BOARD_RUNTIME`、`DEVICE_INTEGRATION`、`POWER_BENCH`、`VEHICLE` 逐项标 PASS / FAIL / NOT_RUN / BLOCKED。

## 6. 当前已知缺口

- `firmware@0a825a2`：仅公共接口、部分纯 C ASW、GPIO EVB 适配和纯 CAN 帧校验；UART/CAN 真正控制器、Timer、设备 CDD 及整个配置/控制链仍需实现。
- 当前仓库的厂商 SDK 是 GPIO 项目携带的**不完整 SDK 子集**，不能臆造 CAN 控制器适配。
- YCT 中 UART1 原始配置为中断模式且 DMA 请求源禁用，不是完成的 UART DMA 配置。
- 产品 PCB 尚无图；真实制动意图、功率回路、标定/算法与失效动作依 P4 继续收集证据。
- App 完整产品功能独立于嵌入式固件测试包，V2 LCD/V3 App 依产品版本另行开发并测试。

## 7. 工作协作与变更约束

- ChatGPT：研究/设计/源码和合同审查、Host CI 与仓库一致性、软件测试包准备。
- Codex：按确定的工作包做受限代码修改和能执行的构建/测试；不自主决定 MCU、控制律、安全动作或更换工程架构。
- 负责人：审批产品和关键接口决策，自行安排 T00～T10 测试、保存证据；无需按每次提交的节奏实测。
- 一次实现工作包 = 需求/接口追踪 + 代码 + 测试 + CMake/Keil 同步 + 文档状态 + SHA，避免重复遗忘 Keil。
- 缺失依据保持可见，不因为一次 Host PASS 宣称实现了完整产品或车规安全。
