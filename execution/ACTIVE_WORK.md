# BrakeSync｜当前执行工作台（2026-11-14）

> **正式基线**：[P0–P8 V0.3](../baselines/2026-11-14_P0P8_V0.3/APPROVAL.md)，[软件质量强制规范 V1.0](../baselines/2026-11-14_SW_STD_V1.0/APPROVAL.md)。版本顺序保持 **V1 真实功能测试 → V2 LCD 产品交付 → V3 App**。
>
> **负责人最新阶段澄清**：Keil+ST-Link 仅为已有初步开发尝试的信息；**现在尚未到环境正式核验、Target工程创建和驱动编码阶段**。不要擅自推进 SW-00/SW-01，也不要把已知尝试当做编译/烧录/运行成功。
>
> **本轮工作**：实施前的软件体系细化：[`BS-SW-PREDEV-001 V0.1`](./software_predevelopment/00_SOFTWARE_ARCHITECTURE_PREP_V0.1_WORKING.md) 与[逐驱动交接工作包](./software_predevelopment/01_DRIVER_MODULE_HANDOFF_V0.1_WORKING.md)；均为 **WORKING / DESIGN_ONLY**，不覆盖正式已批准文档。
>
> **受控档案和交接**：[handover/00_INDEX.md](../handover/00_INDEX.md)；所有资料、环境信息、工作包和证据须进入 GitHub 受控清单，缺失/版权受限在 [缺口登记](../handover/05_MISSING_AND_EXCEPTION_LOG.md) 追踪。

## 当前状态

| 工作面 | 当前实际状态 | 现在允许 |
|---|---|---|
| 产品与安全需求 | 产品参数、真实制动器、危险动作仍有开放项 | 系统需求、故障与接口研究 |
| 软件架构/模块合同 | 已有 P4 系统合同，本轮细化至 ASW/BSW/Driver/Device 模块 | 设计、模块图、接口和任务验收计划 |
| 软件编码与工具环境 | Keil+ST-Link 有初步尝试；新 Host 模板仅 Host 测试 | 仅登记已知事实和未来SW-00验收要求 |
| 正式 Target 工程 | 尚未形成本轮受控 Target 工程/验证结果 | 暂不启动创建/烧录 |
| MCU驱动与设备联调 | 未开始本轮受控实际开发 | 仅设计文件与未来测试用例 |
| 正式产品PCB | 未完成 | PCB输入、功率保护与测试接口需求 |
| 实际制动功率与车辆测试 | 尚无硬件/安全准入 | **禁止接实际制动功率链** |

## 下一批设计任务（当前）

| ID | 任务 | 交付物 | 责任/状态 |
|---|---|---|---|
| PRE-01 | 三版共用软件模块及依赖 | 本轮模块/接口设计工作稿 | ChatGPT编制、负责人复核 / WORKING |
| PRE-02 | SW-ICD-01..05 数据质量与控制边界细化 | 上下游契约、无效输入与状态用例 | ChatGPT / WORKING |
| PRE-03 | 驱动与设备协议逐项任务及测试计划 | 本轮详细交接工作包 | ChatGPT编制、负责人复核 / WORKING |
| PRE-04 | MCU/硬件及产品决策证据清单 | PCB/传感/车辆/故障动作 OPEN 项 | 负责人提供事实；ChatGPT研究 |
| PRE-05 | 三版软件发布范围与LCD功能矩阵 | V1实测、V2 LCD、V3 App要求追踪 | ChatGPT / 后续 |
| PRE-06 | 工具/工程导入与质量门禁清单 | 供未来SW-00/01使用的入场证据要求 | ChatGPT / 仅计划 |
| PRE-07 | 材料归档、移交、外部资料许可闭环 | GitHub清单与受控位置 | ChatGPT/负责人，各自职责 |

## 进入实际开发时才启动的任务

`SW-00 开发环境核验 → SW-01 全新Keil/官方SDK Target工程 → Clock/GPIO/Timer → UART DMA/CAN → 真实设备协议 → ASW制动控制 → V1无功率联调 → 独立审批的功率台架`。

这些当前为 **PLANNED/NOT_RUN**；不能为了填表声称已进行 Target Build、烧录或硬件实测。

## 交付与安全原则

所有代码项目需按照 `BS-SW-STD-001 V1.0` 严格过程受控；旧固件/App不作为基线，Codex不在未获具体源码任务授权时工作。当前管理分支允许归档工作稿，不能把规划文档误作为产品认证或实际制动许可。
