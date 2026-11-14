# BrakeSync｜资料缺口、限制与历史文件处置

**编号**：BS-CM-GAPS-001 · **核查日期**：2026-11-14  
**唯一责任人**：项目负责人；ChatGPT 负责对已获授权的 `management` 分支执行补档与核对。  
**原则**：`IN_CHAT` ≠ `IN_GITHUB` ≠ `HANDOVER_ACCEPTED`。

| 缺口 ID | 资料/原始字节 | SHA-256 | 原因与状态 | 责任方 / 解除条件 |
|---|---|---|---|---|
| GAP-001 | 固件新模板 V0.1 `.zip`，10,620B | `7a24069ba7c86b…` | 本地候选，未在当前 Git Tree 找到同一 Blob；SUPERSEDED / ARCHIVE_NEEDED | ChatGPT：作为历史 HOST-ONLY 样件归档或记录等价源码包 |
| GAP-002 | 固件新模板 V0.2 严格质量门禁 `.zip`，29,467B | `4147c0a026f5de…` | 尚无同一 Blob；HOST-ONLY / ARCHIVE_NEEDED；不可变为正式 `firmware` | ChatGPT：交付只读源码/制品，项目负责人未来授权正式固件导入 |
| GAP-003 | GCC Host 质量门禁原始日志，2,591B | `9ae0ba2ae4e24c60fb5414cb1e8e4c75924f6efe5e9a22e78c50b6e090364c98` | **GITHUB_ARCHIVED / HOST_ONLY**：[GCC14 原始输出](./evidence/GCC14_Host_Strict_V0.2.txt)，仍非 Target/MISRA 证据 | ChatGPT 已归档；接收人为项目负责人 |
| GAP-004 | Clang Host 原始日志，1,266B | `ed08995607fa859c58cd4e898fb9efc58758c7d62ae7983b7810e2a5752d029d` | **GITHUB_ARCHIVED / HOST_ONLY**：[Clang17 原始输出](./evidence/Clang17_Host_Strict_V0.2.txt) | ChatGPT 已归档；接收人为项目负责人 |
| GAP-005 | P1 供应商硬件原始资料 ZIP，5,740,049B | `cf80832d4b662b…` | `RESTRICTED`：公开库，未核实原版再分发许可 | 项目负责人：确定可授权私有存储及供应商许可，GitHub 放索引和哈希 |
| GAP-006 | 云途正式完整 YCT/SDK、工具链、实际 PCB Rev | 未确认 | `NOT_ACQUIRED/NOT_VERIFIED`；SW-00 尚未完成 | 项目负责人提供版本证据，再由 Codex/ChatGPT 核验 |
| GAP-007 | 早期未批准/已被正式基线替代的 Markdown 草稿和页面 | 各文件不同 | `HISTORICAL/SUPERSEDED`：不能视为当前权威设计；本轮仅核对当前已批准材料 | ChatGPT 将来按审计需要归档；不得与批准版混在同一入口 |
| GAP-008 | P0–P8 V0.2 合并 ZIP，195,217B | `a08ac3e7d42748…` | 二进制打包副本未入库；权威 Markdown 与审批记录已在 GitHub，内容没有因此丢失 | 可按完整追溯需求存档，不能代替权威 Git 文档 |
| GAP-009 | 新产品目标 PCB、受控功率硬件、真实模块实测日志 | 暂无 | `NOT_CREATED/NOT_RUN`，不是遗失文件 | 项目负责人、未来授权执行人提供实证 |

### 审计结论

已批准 P0–P8、P3、P4 设计方向、三版发布路径和 `BS-SW-STD-001` 等**已有 GitHub 权威原文**。本缺口清单列的是非权威交付附件、受限第三方字节与尚未产生的工程证据；它们未闭环前，禁止说“所有相关材料已完成交接”。任何填补项都需要更新本记录，附 Git 提交与二次读取证据。
