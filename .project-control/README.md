# BrakeSync 项目管理账本

**状态：ADOPT 接管初始化候选；不是正式项目阶段批准。**

本目录是独立于通用 `ProjectControl/` 管理软件的 BrakeSync 专属记录。建立时间 2026-11-12，唯一管理写入分支 `management`。

读取顺序：`project.json` → `OWNER_DIRECTIVES.md` → `SOURCES.md` → `assets.json` → `ADOPTION_REPORT.md` → `decisions.json` → `management.json` → `tasks.json`。

- `project.json`：项目身份、仓库、ADOPT 候选及快照。
- `assets.json`：**选择性**历史资产清单；当前仅覆盖已核实来源，不是所有对话/所有资料的穷尽清单。
- `decisions.json`：正式决策清单（初始为空）；用户已表达的工作约束在 `OWNER_DIRECTIVES.md`，其有效性和正式批准另行判断。
- `tasks.json`：Codex 执行合同（初始为空，不存在 READY）。
- `management.json`：ProjectControl 通用工作台兼容的初始 DRAFT 数据，不自动授予任何任务/阶段许可。
- `ADOPTION_REPORT.md`：接管发现、证据质量、关键缺口及下一步候选活动。
- `resolve.py`、`workspace_manager.py`：与本次 ProjectControl 4.6.1 包相同的受控任务查询、工作区登记辅助脚本；仓库存在脚本不代表 Windows 实测。

本次 GitHub 基线是管理性事实登记，**不是对完整系统需求、软件架构、产品里程碑的内容审批**。其他分支的研发进展仅观察，不跨分支修改。
