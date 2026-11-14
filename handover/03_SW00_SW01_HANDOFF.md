# BrakeSync｜SW-00 → SW-01 开发环境和全新 Target 工程交接单

**交接编号**：BS-HO-SW00-SW01-001  
**发起时间**：2026-11-14  
**唯一责任人（A）**：项目负责人  
**技术方案及审查执行（R）**：ChatGPT  
**未来目标工程文件执行者（R）**：Codex（待具体任务授权）  
**实物操作、工具确认及接收验收（A/Receiver）**：项目负责人  
**交接当前状态**：`PREPARED / NOT_ACCEPTED / SW00_PARTIAL / SW01_NOT_VERIFIED`

> **负责人新增的直接工程事实（2026-11-14）**：云途开发板已有初步上手与烧录尝试；使用 **Keil（MDK）+ ST-Link**。这是已知的实际工具组合，不再假设必须先改用 GCC/YCT 或其他调试器。**当前仅已确认“尝试使用该组合”，尚未收到成功的 Target Build、Flash Verify、运行/断点和日志证据**，不能标记 SW-01 PASS。

## 已可在 GitHub 取得的输入

1. [P0–P8 V0.3 研发基线](../baselines/2026-11-14_P0P8_V0.3/APPROVAL.md)。
2. [BS-SW-STD-001 汽车电子软件标准 V1.0](../baselines/2026-11-14_SW_STD_V1.0/APPROVAL.md)。
3. [嵌入式软件开发路线](../execution/SOFTWARE_DEVELOPMENT_EXECUTION_V0.1_WORKING.md)。
4. [三版共用系统框架和 PCB 输入 V0.1](../execution/P4_三版共用系统框架与PCB设计输入_V0.1_WORKING.md)。
5. [质量证据工作台](../execution/QUALITY/STATUS_2026-11-14.md) 和 [受控资料索引](./00_INDEX.md)。

## SW-00 开工条件与交付

| 核验项 | 所需实际资料/证据 | 当前结论 | 责任 |
|---|---|---|---|
| 目标 EVB | 实物板号、PCB Rev、MCU 丝印、板级原理图版本 | OPEN；不得从文件名推定实际焊接版本 | 项目负责人提供实物核对 |
| YCT 与 SDK | 官方合法安装包/版本、文档与源码、校验值 | OPEN；历史 SDK 片段不构成完整 SDK | 项目负责人提供；ChatGPT 核对 |
| 工具链 | 已知 **Keil（MDK）+ ST-Link** 用于初步烧录尝试；待确定 MDK 具体版本、Arm Compiler 版本、ST-Link 具体型号及固件版本、驱动、烧录算法/设备包和 OS | PARTIAL / 用户已说明工具组合，版本待核对 | 项目负责人 |
| 最小 Build | 用 Keil（既有尝试路径）建立**全新**可重现 Target 工程，核对官方启动/链接、设备支持包及 Flash Download algorithm；YCT/SDK 来源单独核实 | NOT_VERIFIED；历史开发尝试不能代替新基线 | Codex 需单独任务授权；用户提供工具与板上证据 |
| 实板入口 | ST-Link 与 MCU 的 SWD/JTAG 连线、供电/Vref/GND、识别/下载/校验截图或日志、UART/断点/最小启动 | ATTEMPT_REPORTED；成功与否未提供证据 | 项目负责人实测 |
| 外部模块 | 传感器与蓝牙的供应商型号/资料/修订与接线 | OPEN | 项目负责人提供、ChatGPT 核对 |

## SW-01 验收不能缺少的证据

- [ ] `TARGET_PROJECT_ID`、Board、Variant、官方 SDK/YCT/Keil 工程来源、Keil MDK/Arm Compiler/ST-Link 固件和目标 Flash Algorithm 等版本完整。
- [ ] 全新官方 SDK 工程的生成方法、差异配置和可重复构建命令有记录。
- [ ] Target Build PASS 有可重复的构建日志和版本 SHA；Host PASS 不能替代。
- [ ] 用 **Keil + ST-Link** 分别提供 Target 编译、烧录校验、复位运行和单步/断点的原始证据；不能由“尝试下载”推定已 PASS。
- [ ] 时钟、GPIO、时间基准逐步进入独立驱动任务，正常/异常/恢复测试可追踪。
- [ ] 正式 PCB/制动功率硬件仍未授权输出，开发板保持物理输出隔离。
- [ ] 全部代码修改由明确的 Codex 任务授权；当前管理分支仅交接文件，不向 `firmware` 写入候选模板。

## 交接签收栏

| 字段 | 当前记录 |
|---|---|
| 提交人 | ChatGPT：仅交付规则、输入和清单 |
| 接收人 | 项目负责人 |
| GitHub 基线与质量规范 | 可取得、已批准 |
| 模板/Host 证据归档 | [V0.1/V0.2 原始 ZIP 已入库](./artifacts/MANIFEST.md)；Host 日志亦已入库；接收方校验及正式 Target 工程仍待办 |
| SW-00 完成 | PARTIAL：已知 Keil + ST-Link 的使用尝试；版本、板号、SDK 与连接证据仍 OPEN |
| SW-01 Target Build / Flash / Runtime | NOT_VERIFIED（已有历史烧录尝试，但尚无受控新工程与成功证据） |
| 实物安全准入 | BLOCKED |
| 正式签收 | **PENDING**；须实物/工具和 Target 证据补齐后由负责人确认 |
