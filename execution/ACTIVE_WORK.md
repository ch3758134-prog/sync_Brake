# BrakeSync｜当前执行工作台（2026-11-14）

> **当前项目阶段：P4 系统总体设计与软硬件工程实施准备。** P0–P8 是研发阶段，不是全部已经完成；V1 测试版→V2 LCD交付版→V3 App版是另一个独立的产品版本维度。
>
> **当前正式批准方案**：[P0–P8 V0.3](../baselines/2026-11-14_P0P8_V0.3/APPROVAL.md)，[汽车电子软件工程质量规范 V1.0](../baselines/2026-11-14_SW_STD_V1.0/APPROVAL.md)；产品级量化指标、故障动作与PCB定型仍需真实证据。
>
> **当前 P4 交付（WORKING/待负责人系统级评审）**：[整机功能架构和软硬件分配 V0.1](./P4_system_architecture/00_SYSTEM_ARCHITECTURE_AND_ALLOCATION_V0.1_WORKING.md) 与 [PCB设计输入及准入矩阵 V0.1](./P4_system_architecture/01_PCB_INTERFACE_AND_READINESS_V0.1_WORKING.md)。
>
> **不要提前进入软件实施**：Keil + ST-Link 是负责人此前初步使用尝试的已知信息，不代表当前已开始SW-00/01。早前 [软件实施前设计文档](./software_predevelopment/00_SOFTWARE_ARCHITECTURE_PREP_V0.1_WORKING.md)及其深入接口/测试工作稿作为**后续参考（PARKED / NOT_APPROVED_AS_IMPLEMENTATION）**保留，不主导当前系统阶段。

## 一、项目真实进度

| 阶段/产品 | 当前结论 | 下一必备证据 |
|---|---|---|
| P0～P2 | 研发路线、需求方向已批准；定量产品事实未齐 | 市场、目标车辆、制动器与供电等实物证据 |
| P3 | 系统概念及危险场景初筛批准，条件性收口 | 正式产品危害分析和受控功能/物理边界 |
| **P4** | **当前进行系统级架构/软硬件责任/PCB设计输入**，新稿待审 | 负责人核实实体产品事实，进行系统审查并逐项闭环 |
| P5 | 尚未正式进入本轮新工程、驱动、PCB实现 | P4输出及具体工程授权，SW-00/01客观核验 |
| P6～P8 | 集成/验证/发布计划已批准但实际未实施 | 每一版本真实构建、实测、验证、发布证据 |
| V1 | 真实基础功能测试版尚未完成 | 真实模块/受控功率台架按安全准入逐步验证 |
| V2 | LCD首个产品交付版尚未开始完成实现 | LCD功能矩阵、正式PCB/产品与测试 |
| V3 | App版排在V2后 | 继承共用核心、无线/应用安全与回归 |

## 二、当前正在推进的系统工程任务

| ID | P4 当前任务 | 已形成的输出 | 仍缺关键材料 | 责任/状态 |
|---|---|---|---|---|
| P4-SYS-01 | 整机外部边界、功能链与安全职责 | [系统架构](./P4_system_architecture/00_SYSTEM_ARCHITECTURE_AND_ALLOCATION_V0.1_WORKING.md) | 车端供电/信号、制动器与使用场景 | ChatGPT编制；项目负责人待复核 / WORKING |
| P4-SYS-02 | SF-01～09 硬件/软件/外部/测试责任分配 | 同上（第2节） | 实物拓扑与故障检测能力 | WORKING |
| P4-HW-01 | PCB-01～13 设计输入、硬件接口与工艺测点 | [PCB准入矩阵](./P4_system_architecture/01_PCB_INTERFACE_AND_READINESS_V0.1_WORKING.md) | E-PROD/E-BRAKE/E-ELEC/E-MOTION/E-SAFE/E-HMI | WORKING；功率相关BLOCKED |
| P4-PROD-01 | C-01～C-06 集中产品取舍 | 已有一致性清单 | 首发市场/车型/轮端/供电/输入及V2 LCD实物方案 | OPEN，负责人集中决定 |
| P4-SAFE-01 | 失效后真实功率动作与保护责任的系统分析准备 | HA/SG/SSR初筛+本轮硬件责任划分 | 正式危害分析、硬件FMEA/故障注入与适用车辆 | OPEN/BLOCKED |
| P4-VAL-01 | V1测试版的实物验证层次及专用台架输入 | P4/V1验证路径已规划 | 实物硬件/台架安全准入、测量方案 | PLANNED，不是测试通过 |
| CM-01 | 工程资料受控归档、交接、版权/缺口 | [唯一资料入口](../handover/00_INDEX.md)和[总账](../handover/02_ARTIFACT_REGISTER.md) | 外部授权材料等OPEN项 | 继续跟踪 |

## 三、当前暂停而不是删除的工作

- `SW-00/SW-01` 环境核验、全新Keil工程、Target编译、烧录、GPIO/CAN/UART DMA驱动：**未获本轮开工授权**，按未来受控任务进行。
- `execution/software_predevelopment/` 的软件模块、接口及测试追踪材料：**存档待将来软件系统设计使用**，不因存在文件就宣称P5启动或软件详细设计已冻结。
- 现有旧固件/App源码不进入正式新项目基线，也不作为当前研究或修改范围。

## 四、硬性约束

1. 研发 EVB 不接真实拖挂制动功率；任何有关实际输出的工作必须独立硬件/安全/台架准入。
2. 故障不能武断统一“立即清零”，需按目标车辆与工况做危险分析。
3. 不虚构 PCB引脚、功率电流、通信参数、车辆质量、响应时限或安全认证。
4. 阶段分开标记：方案批准≠设计冻结≠代码完成≠Host/Target Build≠烧录≠实物验证≠产品发布。
5. 每次设计文件交付均纳入GitHub `management` 资料索引/总账，保留负责人、编制者、版本及签收状态；第三方原件受公开仓库许可约束。
