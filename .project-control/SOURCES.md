# BrakeSync 接管来源地图（V0.1）

## 本轮已能核实的范围

- 直接用户指令：本对话对管理分支、角色和建立管理基线的要求；项目长期工程规则。
- 项目历史对话：当前环境提供的**五组项目对话摘要/可见用户消息片段**，话题包括总体开发、云途驱动准备、App 职责、蓝牙说明、GitHub 平行分支。**没有取得这些对话的全部逐轮全文**，因此本记录不能称“全部对话原文已通读”。
- GitHub：`ch3758134-prog/sync_Brake` 的 `main` / `firmware` / `app` / `management` 分支目录树、相关 README、SDK 审计、源码、测试与 CI 工作流文件；截至 2026-11-12 观察的专业 HEAD 分别为 `d1f38b250f2c229aeb122985dc0520b969fe971d` 和 `5b911b5b57352fb1c8d0017faeb4b00ace078eb0`。
- 历史产品材料：在 ChatGPT 可访问资料中查到并阅读 `拖挂式房车制动同步器_产品方案设计说明_V1.0.pdf`、`RV_Brake_Controller_User_Function_Plan_V2.5.pdf`、`rv_context_v5.pdf`。这些资料不在当前 GitHub 仓库内，本轮不能核实其正式批准历史。
- 硬件资料线索：历史对话有 `BST-BMA510-DS000-03.PDF`、`WLT8016模块规格书_F1_15_Rev2.1.pdf`、云途 SDK 部分压缩包及 CAN/串口相关记录；可见 SDK 核验文本来自 `firmware/board/ytmicro/SDK_AUDIT.md`，不是本轮完整 SDK 实物复审。

## 精确 Git 证据入口

- 固件：`https://github.com/ch3758134-prog/sync_Brake/tree/firmware/firmware` @ `d1f38b250f2c229aeb122985dc0520b969fe971d`
- App：`https://github.com/ch3758134-prog/sync_Brake/tree/app/app` @ `5b911b5b57352fb1c8d0017faeb4b00ace078eb0`
- 管理系统：`https://github.com/ch3758134-prog/sync_Brake/tree/management/ProjectControl` @ `5b60f8822619616287507464479846d0428c7b6c`
- 中性主线：`https://github.com/ch3758134-prog/sync_Brake/tree/main`

## 有意保留的证据限制

1. **无法直接逐轮读取所有历史 ChatGPT 对话**：已优先保留可见用户指令与已有项目资料；如涉及某一版决策，仍须找原始记录核对。
2. 仓库 PR/Issue 查询为空不代表外部研发资料不存在；CI workflow 文件存在不等于 GitHub Actions/Target Build PASS。
3. 历史 PDF 是来源而不是授权；后续文档要逐条标识 OBSERVED（确切可核查事实）、REPORTED（用户或旧资料说明）、SUPPORTED_CANDIDATE（当前暂作依据）、UNKNOWN 与 CONFLICT。
4. 资料变化只复核受影响内容，不重置无关模块。
