# BrakeSync｜SW-00 → SW-01 开发环境和全新 Target 工程交接单

**交接编号**：BS-HO-SW00-SW01-001  
**发起时间**：2026-11-14  
**唯一责任人（A）**：项目负责人  
**技术方案及审查执行（R）**：ChatGPT  
**未来目标工程文件执行者（R）**：Codex（待具体任务授权）  
**实物操作、工具确认及接收验收（A/Receiver）**：项目负责人  
**交接当前状态**：`PREPARED / NOT_ACCEPTED / SW00_OPEN / SW01_NOT_RUN`

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
| 工具链 | IDE、编译器版本/许可、调试器、烧录软件、操作系统 | OPEN | 项目负责人 |
| 最小 Build | 全新官方生成的 Target 工程、启动文件、链接脚本、受控配置 | NOT_RUN | Codex 需单独任务授权 |
| 实板入口 | 供电/USB/Debug 安全接线、UART 日志、最小启动 | NOT_RUN | 项目负责人实测 |
| 外部模块 | 传感器与蓝牙的供应商型号/资料/修订与接线 | OPEN | 项目负责人提供、ChatGPT 核对 |

## SW-01 验收不能缺少的证据

- [ ] `TARGET_PROJECT_ID`、Board、Variant、SDK/YCT 版本、工具链版本完整。
- [ ] 全新官方 SDK 工程的生成方法、差异配置和可重复构建命令有记录。
- [ ] Target Build PASS 有可重复的构建日志和版本 SHA；Host PASS 不能替代。
- [ ] 烧录与调试分别有工具版本、串口/断点、目标设备证据。
- [ ] 时钟、GPIO、时间基准逐步进入独立驱动任务，正常/异常/恢复测试可追踪。
- [ ] 正式 PCB/制动功率硬件仍未授权输出，开发板保持物理输出隔离。
- [ ] 全部代码修改由明确的 Codex 任务授权；当前管理分支仅交接文件，不向 `firmware` 写入候选模板。

## 交接签收栏

| 字段 | 当前记录 |
|---|---|
| 提交人 | ChatGPT：仅交付规则、输入和清单 |
| 接收人 | 项目负责人 |
| GitHub 基线与质量规范 | 可取得、已批准 |
| 模板/Host 证据归档 | 见缺口台账；未全部闭环前不得标记 complete |
| SW-00 完成 | NO |
| SW-01 Target Build / Flash / Runtime | NOT_RUN |
| 实物安全准入 | BLOCKED |
| 正式签收 | **PENDING**；须实物/工具和 Target 证据补齐后由负责人确认 |
