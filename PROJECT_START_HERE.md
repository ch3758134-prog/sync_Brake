> **当前有效状态更新（2026-11-13）**：P0 V0.1、P1 V0.2、P2–P8 V0.1 全阶段方案已获负责人批准并归档。最新基线入口：[已批准 P0–P8 方案](./baselines/2026-11-13_P0-P8_V0.1/README.md)；实时工作入口：[执行工作台](./execution/ACTIVE_WORK.md)；SDK/Target 调查：[FW-01](./execution/SDK_TARGET_READINESS.md)。固件和 App 分支仍只读，实际制动功率禁能。

---

> **下方为 2026-11-12 早期 ADOPT 候选快照说明，保留以便追溯。其“未批准”等内容已被以上最新审批事实取代。**

# BrakeSync｜项目入口（管理基线候选 V0.1）

> 目标仓库：`https://github.com/ch3758134-prog/sync_Brake.git`。本文件只定义项目管理入口与已知工作边界，**不是制动产品的正式发布、需求批准或可执行任务授权**。
> 记录日期：2026-11-12。管理分支：`management`。

## 这是什么项目

BrakeSync 是面向拖挂式房车的电子同步制动控制系统，目标是形成可产品化、可维护、可验证、兼顾批量成本的软硬件技术平台。制动控制由主机独立完成；LCD 或手机 App 用于配置、状态与故障交互。具体产品配置及安全参数须以有效设计和验证结果为准。

## 分支与角色

- `main`：中性仓库入口，不作为正在开发的代码接收分支。
- `firmware`：MCU / ASW / BSW / 设备驱动，专业研发维护；管理 AI 只读。
- `app`：手机 App / BLE / 模拟与测试，专业研发维护；管理 AI 只读。
- `management`：项目管理 AI **唯一允许写入的分支**；通用制度在 `ProjectControl/`，本项目账本在 `.project-control/`。
- 用户是项目负责人、产品与关键技术决策人、审阅与验收人；ChatGPT 管理 AI 负责证据恢复、协调与管理；专业 AI 负责领域研究/设计；Codex 只执行明确授权的边界内任务。

## 按什么顺序读

1. 阅读本入口和 `.project-control/README.md`。
2. 查阅 `.project-control/project.json`、`assets.json`、`decisions.json`、`management.json`、`tasks.json`。
3. 阅读 `.project-control/OWNER_DIRECTIVES.md`、`ADOPTION_REPORT.md` 和 `SOURCES.md`，理解已核实事实、用户原话的可用范围及待确认事项。
4. 阅读 `ProjectControl/START_HERE.md`、`portable/INITIALIZATION_PROTOCOL.md`、`portable/DELIVERY_REVIEW_PROTOCOL.md`、`portable/TASK_CONTRACT.md`。该 ProjectControl 4.6.1 包标识为 **CANDIDATE_NOT_RELEASED**。
5. 需要讨论研发状态时读取各专业分支的准确 HEAD 与文件，不能把分支最新代码当作最终产品需求或批准。

## 安全与状态边界

- 当前代码尚无整车制动输出验收；**任何未验证平台不得释放真实制动功率输出**。
- 区分 `接口/源码存在`、`Host 测试`、`目标芯片编译`、`烧录`、`实物功能验证`、`系统/产品安全验证`。
- 决策记录 `decisions.json` 初始为空；历史用户指令另列，不冒充正式 Gate/设计批准。
- `tasks.json` 初始无 READY；Codex 不能因看到管理文件就直接开工、push、merge、reset 或删除工作区。
- 正式研究、设计、计划等候审成果须先向负责人完整呈现、获得具体版本的发布许可；候审发布、正式批准和实施授权相互独立。

## 本地 Codex 注意

管理分支仅放管理系统及项目记录，不是固件或 App 的代码工作基线。Codex 必须核对目标仓库、被授权专业分支、任务合同、有效输入、文件范围和 HEAD；`python .project-control/resolve.py select` 无 READY 时按拒绝执行处理。用户指定本地目录；不得从本仓库推断本地 Windows 路径。
