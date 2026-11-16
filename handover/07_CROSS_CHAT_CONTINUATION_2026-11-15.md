# BrakeSync｜跨对话完整受控上下文交接与 SW-00 接续记录

**文件编号**：BS-HO-CROSS-CHAT-001  
**版本**：V0.1（2026-11-15，America/Los_Angeles）  
**性质**：已有批准设计、事实、决策、未决项、资料及后续任务的**交接索引与状态快照**；不是新技术方案审批、P4 结项或 Target/制动输出许可。  
**核查来源**：本项目可见对话上下文 + GitHub \`ch3758134-prog/sync_Brake\` 的 \`management\` 分支受控原文。交接核查起点 HEAD：\`26311884461c1a5732d973b8655f946967542f08\`。  
**准确性边界**：不可逐字读取另一个 ChatGPT 对话的全部消息；因此“完整交接”指对**当前可恢复的全部批准基线、关键指令、索引及未决事项**建立可追踪入口，而非声称已取得逐字聊天全文。批准结论以有版本锁定的原文与 \`APPROVAL.md\` 为准。  
**最终人类负责人/验收人**：项目负责人；ChatGPT 为研究、架构、文档和审查角色；Codex 为获得明确任务合同后的源码、构建、测试执行者。

## 1. 最高优先级继续约束

1. BrakeSync 是产品化拖挂房车电子同步制动系统，不是开发板 Demo。共用制动控制核心，按功能配置 Variant、硬件平台 Board、软件版本 Release **彼此独立**管理。
2. 产品路线 **V1/R-TEST 真实基础功能测试版 → V2/R-LCD LCD 首个交付产品 → V3/R-APP App 产品**。研发 P0～P8 是工程阶段，不是 V1～V3；App 研发对话可以做准备，但 V1/V2 不等待 App 才能制动。
3. **旧 firmware/app 代码排除出全新实施基线**。旧仓库与 Host-only ZIP 可作为历史归档，不按“已写过”计入新 Target 进度；未来清理/复用另评审，不破坏性删除。
4. 新交付以**本 ChatGPT 对话直接审阅的 Markdown**为主，GitHub management 归档；不再制作审阅 HTML。
5. 已批准 P4 系统方向和 2026-11-15 阶段审阅；负责人强调**实测证据驱动重新审查/修订已批准内容**。修订需新版本、变更原因、影响范围、证据、复测和新批准；不得覆盖原审批字节。
6. 新固件按强制质量基线 \`BS-SW-STD-001 V1.0\`：C、可读英文名、必要中文注释、ASW/BSW、严格需求/接口/测试追踪、GCC/Clang/Target 告警与静态分析、MISRA C:2025/ISO 26262:2018/Automotive SPICE 4.1/ISO/SAE 21434:2021 等**工程对标**。不能声称已认证、ASIL 已定或 MISRA 已完全通过。
7. **真实制动功率始终禁止**：当前 EVB 不接轮端功率/车辆制动链，不以软件 PWM=0 或看门狗替代实体安全；功率台架、整车测试分别需要危险分析、物理独立保护、隔离/限能/急停和单独准入。
8. 管理 AI 只写 \`management\`，其他 \`firmware\`、\`app\`、\`main\` 只读；不得将已有计划审批自动解释为具体文件修改或 Git 合并许可。

## 2. 当前总体阶段事实

| 阶段 | 已批准/已归档 | 仍然未完成 |
|---|---|---|
| P0 项目立项与目标 | 项目方向、独立主控、共用平台、负责人角色 | 产品市场及可宣称范围需要实证 |
| P1 现状/可行性 | 真实开发板与外部材料作为**研究资料**的索引、证据和缺口管理 | 实物 PCB Rev/MCU、正式 SDK、模块身份及产品硬件真实性仍 OPEN |
| P2 系统需求 | 需求框架、产品版本分配、追踪与验证方法 | 重量、轴数、电压、电流、性能与完整法规/安全指标未冻结 |
| P3 系统定义 | SF-01～09、PE-01～06、KPI-01～13 字典、HA-01～12/SG-01～08/SSR-01～18 **初筛** | 正式 HARA/FMEA、实体故障后动作及车辆范围未闭合 |
| **P4 当前主阶段** | DD-01～08、SYS-IF-01～08、PCB-01～13、TRIG/MOT/PWR 方案比较、FC-01～12/SP-01～08/TV-01～18 **设计与计划获批** | P4 **未整体结项**；真实 PCB/负载/故障保护/功率实证均未冻结 |
| P5 新一轮实现 | SW-00/SW-01 起步方案、软件质量规范和历史 Host 模板存档 | **全新** Target 工程、外设驱动、板上运行、业务实现尚未实证完成 |
| P6 集成 | I0～I4 集成门槛计划 | 本轮新工程未完成实际系统集成 |
| P7 验证 | V0～V4 验证及各阶段证据规则已规划 | 全部真实功率/整车验证未完成；SWTC/TV 不能写 PASS |
| P8 发布与维护 | R0～R2/三版发布治理计划 | V1、V2、V3 都不能宣称已交付产品 |

## 3. 正式批准基线（可逐份恢复的完整原文）

- [P0～P8 早期全阶段材料与审批](../baselines/2026-11-13_P0-P8_V0.1/00_总入口_批准状态与使用说明.md)：P0、P1、P2～P8 的原始文本/审批历史，后续与 V0.2/V0.3 不冲突的内容继续有效。
- [P3 功能与边界 V0.2 批准](../baselines/2026-11-13_P3_SYS_V0.2/APPROVAL.md)；[产品适用包络 V0.1](../baselines/2026-11-13_P3_ENVELOPE_V0.1/APPROVAL.md)；[危险初筛 V0.1](../baselines/2026-11-14_P3_SAFETY_SCREEN_V0.1/APPROVAL.md)。
- [P3～P8 系统准备与实施条件 V0.1](../baselines/2026-11-14_P3P8_CONSOLIDATION_V0.1/APPROVAL.md)。
- [P0～P8 集中审批 V0.2（含 DD-01～08）](../baselines/2026-11-14_P0-P8_CENTRAL_APPROVAL_V0.2/APPROVAL.md)。
- [P0～P8 当前统一研发总路线 V0.3](../baselines/2026-11-14_P0P8_V0.3/APPROVAL.md)（发生冲突时优先于 V0.2 的路线描述）。
- [P4 整机/PCB 系统设计输入 V0.1 有条件批准](../baselines/2026-11-14_P4_SYSTEM_DESIGN_V0.1/APPROVAL.md)；其中原始架构位于 [系统架构与责任分配](../execution/P4_system_architecture/00_SYSTEM_ARCHITECTURE_AND_ALLOCATION_V0.1_WORKING.md)，PCB输入位于 [PCB准入矩阵](../execution/P4_system_architecture/01_PCB_INTERFACE_AND_READINESS_V0.1_WORKING.md)。
- [P4 物理方案比较 V0.1 批准](../baselines/2026-11-14_P4_PHYSICAL_OPTIONS_V0.1/APPROVAL.md)；[P4 外部接口/资料证据合同 V0.1 批准](../baselines/2026-11-14_P4_EXTERNAL_INTERFACE_V0.1/APPROVAL.md)。
- [P4 故障链及分层验证计划 V0.1 批准](../baselines/2026-11-15_P4_FAULT_CHAIN_V0.1/APPROVAL.md)；[**最新 P4 阶段审阅批准**](../baselines/2026-11-15_P4_STAGE_REVIEW/APPROVAL.md)。
- [软件工程强制质量规范 V1.0 批准](../baselines/2026-11-14_SW_STD_V1.0/APPROVAL.md)。

**解释优先级**：最近用户明确指令 > 当前已批准新版本 > 在新版本未覆盖且不冲突的旧版已批准合同 > 工作稿/历史工程示例。审批前原文标题中的 DRAFT、WORKING、NOT_APPROVED 只代表其编制时状态，以相应 APPROVAL 为准。

## 4. P4 系统设计与未决物理边界

- **制动控制责任**：牵引车/驾驶员意图、拖挂运动数据 → 可信度/时效/版本一致的输入快照 → ECU 独立逻辑请求 → 执行许可/实体保护 → 功率电流/轮端实际制动。上述阶段分别建模并独立验证。LCD/BLE/App 负责交互、设置和显示，不处于自动制动的实时控制必经链。
- **DD-01～08**：产品包络；制动意图与运动；请求/许可/反馈； INIT/INHIBITED/READY/ACTIVE/DEGRADED/FAULT/SERVICE 状态与诊断；端到端 KPI 与测点；重量档和制动力参数事务；安全/验证追踪；Variant/Board/Release 独立配置。
- **配置语义**：RECEIVED/VALIDATED/ACCEPTED/EFFECTIVE/PERSISTED/READBACK_CONFIRMED 分离；重量档与 1～10 制动力微调是不同维度，具体档数、标定、保存及行驶时配置许可仍 OPEN。
- **SYS-IF-01～08**：车端触发、运动传感、逻辑/功率边界、轮端负载、保护与反馈、HMI/维护、供电接地、应急驻车/脱挂。04/05/08 中真实功率关联的物理决策 BLOCKED。
- **PCB-01～13**：供电、车端输入、传感、真实功率、复位默认态、独立许可、反馈、隔离测点、LCD/BLE、编程调试、热/EMC/应急连接的设计输入；**不是已经审查或生产的产品 PCB**。
- **TRIG-A～D、MOT-A～C、PWR-A～C**：当前只有对比维度及所需证据，没有选中具体拓扑。BMA510 芯片与外接 CAN 加速度模块不得当作同一个设备。
- **安全计划**：HA-01～12、SG-01～08、SSR-01～18、SAFE-01～07、FC-01～12、SP-01～08、TV-01～18 是研究/设计/计划，不是完成了 HARA、硬件 FMEA、ASIL 或故障注入测试。特别禁止把任一失效情形一概设成“立即清零输出”，需危险场景/物理制动后果证据。

## 5. 软件分层、接口及测试交接

- **ASW**：制动输入语义和质量、运动信号质量、控制请求、运行状态/诊断、配置/参数事务、Variant/版本兼容。
- **BSW**：官方 MCU SDK、Board/Clock/PinMux/IRQ、基础驱动（GPIO/Timer/UART DMA/CAN 等）、外部设备适配（真实 CAN 运动模块/BMA510/蓝牙）、时间/NVM/看门狗等必要服务。
- **接口合同**：参考 [SW-ICD-01～05](../execution/software_predevelopment/02_FIVE_SOFTWARE_INTERFACE_CONTRACTS_V0.1_WORKING.md)，分别为制动意图、运动信息、一致性快照/逻辑请求、请求许可/执行反馈、状态/诊断/配置与 HMI。为软件后续设计参考，不当作所有参数/行为已冻结。
- **追踪与测试**：[SWR-01～15、SSR-01～18 → SWTC-01～34](../execution/software_predevelopment/03_REQUIREMENT_INTERFACE_TEST_TRACE_V0.1_WORKING.md)。SWTC-01～30可设计 Host/非功率测试；SWTC-31～34涉及真实执行/车辆的准入继续阻断。
- **实现风格**：主函数只负责初始化和任务调度，IRQ/DMA 收发与状态错误处理必须在模块自身；ASW 不直接依赖芯片寄存器；不为每个官方 SDK API 机械地增加多层空壳；确定性、静态资源、有界缓冲、故障路径及实测可追踪。
- **任务编号注意**：\`execution/SOFTWARE_DEVELOPMENT_EXECUTION_V0.1_WORKING.md\` 与后续**强制** \`BS-SW-STD-001 V1.0\` 对 SW-02 及后续驱动的编号分组存在差异。新代码合同应优先采用 **SW-00 环境、SW-01 Target，后续按已批准质量规范 SW-02 时钟/复位/引脚/GPIO、SW-03 Timer、SW-04 UART DMA、SW-05 CAN、SW-06 设备、SW-07 NVM/看门狗/保护接口、SW-08 ASW、SW-09 无功率联调、SW-10 功率台架（BLOCKED）**；历史细分清单仅作模块明细，不将两套 SW 号混用。

## 6. 硬件、SDK 与交付证据

| 材料/事实 | 已知 | 不可推断/下一证据 |
|---|---|---|
| 云途 EVB 原理图 | \`SCH_YTM32B1LE0_EVB_Q64_RevC.pdf\`，5页，SHA-256 \`65c9198ac5f31b2f58efb6e8af4205fafb92c7277ed54b68f6063969264f41c6\` | 实物是 RevC、实物 MCU 顶标与图一致均**未确认** |
| EVB 手册 | \`YTM32B1LE0-EVB-LQFP64评估板参考手册.pdf\`，RevA，16页，SHA-256 \`bfd1af667d38b3fb2f73d469150af3162a877b604a1503fcaa0679616b432c04\` | RevA 与 RevC 的 I2C/引脚定义冲突，未实物核验不可照搬 |
| BMA510 原始手册 | \`BST-BMA510-DS000-03.PDF\`，174页，SHA-256 \`0eb1c96383b37a31604ad75ea4e389a96390c31426b783915268fe1d8a1e8099\` | 不是已购外置 CAN 运动模块的身份证据 |
| 蓝牙 | 历史 \`WLT8016模块规格书_F1_15_Rev2.1.pdf\` 及其它候选模块资料 | 真实模块型号、固件、AT/透传、GATT、权限和协议未固定 |
| 开发工具 | 负责人曾用 **Keil MDK + ST-Link** 做初步使用与烧录尝试 | 编译器/工具版本、Flash Algorithm、Target Build/Flash Verify/Runtime 证据未取得 |
| SDK/新工程 | 旧工程有 \`YTM32B1LE0_1_5_0\` 片段/旧 PC 绝对路径；真实新版 SDK/生成配置待核验 | 旧 SDK 残片不构成完整官方包，不能凭空编外设 API |
| Host-only 模板 | [V0.1/V0.2 原始 ZIP 及校验](./artifacts/MANIFEST.md) 与 [GCC/Clang Host 证据](./evidence/) 在 management 已归档 | 不是新的 Target 工程或功能实测；接收确认仍 OPEN |
| 供应商原件 | [硬件来源索引](../assets/hardware/README.md) 已有版本和校验；公开仓库不公开上传未核对许可的 PDF | [Issue #2](https://github.com/ch3758134-prog/sync_Brake/issues/2) 合法私有保管和实际签收仍 OPEN |

资料签收与缺口：[统一资产交接总账](./02_ARTIFACT_REGISTER.md)、[缺口/例外](./05_MISSING_AND_EXCEPTION_LOG.md)、[SW-00/01交接单](./03_SW00_SW01_HANDOFF.md)。原始二进制、当前实板实测和 P4 设计批准是三个不同等级的证据。

## 7. 产品集中未决项、暂不阻塞独立软件任务

- C-01：首发市场、目标牵引/拖挂车辆、质量范围、轴数。
- C-02：真实电制动器、轮端数量与负载额定、线束、电热参数。
- C-03：牵引车制动意图物理来源、运动传感器身份/安装/协议与主辅优先关系。
- C-04：V2 LCD/按键/PCB/外壳及功能矩阵。
- C-05：重量档/制动力微调的标定与保存、生效、行驶中配置和失效规则。
- C-06：手动/Boost/倒车/OTA/侧摆等候选增强能力版本归属。
- 实际车端电源瞬态、功率硬件拓扑、保护默认态、反馈覆盖、脱挂应急/驻车、性能定量 KPI、正式 HARA/FMEA/法规适用性均 OPEN/BLOCKED。

## 8. 本次交接后的真实继续执行入口

### SW-00｜当前开始证据整理，未完成实体核验

**从已有材料确认**：候选 YTM32B1LE0 EVB 的 RevC 图、RevA 手册、BMA510 PDF 身份和校验；负责人使用过 Keil + ST-Link 的尝试事实；旧 SDK 工程只是残片。上述属于 \`SW00_PARTIAL\`，不能标记完成。

**下一次真实工作需收集的最小信息**：实物板号/PCB Rev、MCU 顶标、实际已接 SWD 和供电、Keil/Arm Compiler/ST-Link/Device Pack/Flash Algorithm 版本、官方 SDK/YCT 的包名/来源/版本/哈希/示例工程，以及新工程最小 Build/Flash/Runtime 的原始日志或截图。由此核对工具、启动/链接、真实引脚、时钟；不要为了缺资料而编造 API。

### SW-01｜全新无功率 Target 工程；当前 NOT_RUN

使用所核对的官方完整 SDK/YCT 或适用的官方 Keil 工程模板、对应实物 MCU 和精确工具版本**新建工程**。第一批只做启动/复位、最小 \`main\`、版本及构建标识，严格没有真实制动功率连接。验收分为 \`TARGET_BUILD_PASS\`、\`FLASH_PASS\`、\`TARGET_RUNTIME_PASS\`，各自提交独立证据；旧 firmware/app 不自动继承。

### SW-02 及后续｜在证据满足后逐模块实测

按强制质量规范模块顺序：Clock/Reset/PinMux/GPIO → Timebase/Timer → UART DMA TX/RX + IRQ + 超时/溢出/错误恢复 → CAN 双节点/Bus-off → 真实传感器与蓝牙协议 → NVM/Watchdog/诊断 → ASW/Host 与 Target 无功率集成。独立接口与模块测试，不把中断/协议全部塞进主函数。每个模块均保留源码提交、编译日志、测试条件、原始通信/测量结果、异常路径和未完成项。

### SW-10 功率台架｜继续 BLOCKED

必须等产品实际负载、电气设计、HARA/FMEA、独立输出保护与测试台架隔离/限能/急停经单独审查，不能因本次交接或 P4 批准而解锁。

## 9. 协作、变更与证据的长期记法

- 用户：产品/架构决策、真实开发板与台架操作及验收；可随时针对实测提出重评。
- ChatGPT：根据**新证据**审查既有决策是否仍正确；明确受影响的 DD/SYS-IF/SSR/SWR/SWTC、改动依据、测试判据和具体 Codex 合同。
- Codex：只在明确批准文件范围/分支/命令和禁改项后执行文件改动、Build、静态检查/测试、产生 diff/日志/提交；不自主决定功率安全策略。
- 状态字典严格分开：\`DESIGN_APPROVED\`、\`SOURCE_PRESENT\`、\`HOST_BUILD_PASS\`、\`HOST_TEST_PASS\`、\`TARGET_BUILD_PASS\`、\`FLASH_PASS\`、\`TARGET_RUNTIME_PASS\`、\`MODULE_HW_PASS\`、\`INTEGRATION_PASS\`、\`POWER_BENCH_PASS\`、\`VEHICLE_PASS\`。
- 遇到新实测：**测试对象/版本 → 预期与实测 → 日志/波形 → 关联需求/设计/实现 → 影响分析与新版本 → 审阅批准 → 复测 → 状态更新**。永久保留原审批内容，禁止事后更改历史原文。

## 10. 完整性与例外说明

- 已核对的权威状态是：**P4 阶段总览于 2026-11-15 已批准，P4 未结项，产品功能与安全数值仍 OPEN，软件可转入独立无功率工作**。
- 当前无需再次反复批准此前 P0～P4 原则；但是**没有凭这次交接完成实板 SW-00/01，也没有在本记录中改写 firmware/app/main 或运行 Keil/Target**。
- 早期主工程 \`KF32A136\` 以及某些历史 Host 代码文档不是当前云途 EVB 工程依据。当前候选云途型号/物理总线能力仍必须以实物+官方 SDK 重新核验。
- 全部已批准原文/报告保存在上述 Markdown 链接及 [交付总入口](./00_INDEX.md)；不复制全量历史原文到本摘要，避免丢失版本、SHA 和审批效力。跨对话无法逐字取得的聊天历史保持“未直接读取”，禁止声称逐字无遗漏。
