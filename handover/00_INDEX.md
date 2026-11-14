# BrakeSync｜受控资料与项目交接唯一入口

**日期**：2026-11-14  
**项目责任人**：项目负责人（单人研发）；ChatGPT/Codex 是受控 AI 执行角色。  
**GitHub 仓库**：`ch3758134-prog/sync_Brake`，当前工作权限仅 `management`。  
**状态说明**：文档归档与实际固件/硬件实现、烧录和产品发布是不同的事实。

## 当前权威入口

- [P0–P8 研发方案 V0.3 批准记录](../baselines/2026-11-14_P0P8_V0.3/APPROVAL.md)：V1 真实功能测试 → V2 LCD 交付 → V3 App。
- [汽车电子软件工程与 C 编码规范 V1.0 批准记录](../baselines/2026-11-14_SW_STD_V1.0/APPROVAL.md)：编码、质量门禁和验证要求。
- [当前研发工作台](../execution/ACTIVE_WORK.md)：SW-00 环境核实 → SW-01 官方 YCT/SDK 新工程 → 驱动。
- [项目决策记录](../.project-control/decisions.json)：批准记录及开放事项。
- [文档、制品与任务交接控制规程](./01_DOCUMENT_CONTROL.md)。
- [交付物登记总账](./02_ARTIFACT_REGISTER.md)。
- [SW-00/SW-01 单项交接包](./03_SW00_SW01_HANDOFF.md)。
- [外部/供应商原件控制登记](./04_EXTERNAL_REFERENCE_REGISTER.md)。
- [未交付/未上库缺口清单](./05_MISSING_AND_EXCEPTION_LOG.md)。

## 关键说明

当前 `management` 早期已归档的批准文档和批准原件保持只读；本轮补充的是**控制制度与完整性登记**。V0.1/V0.2 本地固件模板以及第三方原始资料是否全部进入 GitHub，以缺口清单和后续二次核验为准，不能因建立索引而宣称全部移交完成。

**严禁**将测试版软件或当前 EVB 连接到未经安全准入的真实拖挂制动功率链。
