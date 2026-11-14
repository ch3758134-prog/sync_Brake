# BrakeSync｜历史固件模板候选制品归档

**Artifact ID**：BS-SW-TEMPLATE-HOST-ARCHIVE-001  
**日期**：2026-11-14  
**版本状态**：`ARCHIVED_HISTORICAL_HOST_ONLY`  
**最终责任/接收**：项目负责人（唯一实际人类负责人）  
**原件编制与本次归档执行**：ChatGPT（AI 执行）；实板 Target 工程实施由后续单独授权的执行者承担。  
**相关事项**：[GitHub Issue #1](https://github.com/ch3758134-prog/sync_Brake/issues/1)。

## 归档原件及逐字节核验

| 原本交付文件 | 当前文件 | 字节 | SHA-256 | Git Blob SHA-1 |
|---|---|---:|---|---|
| `BrakeSync_全新固件工程模板_V0.1.zip` | [V0.1 候选模板](./BrakeSync_firmware_template_V0.1_HOST_ONLY.zip) | 10620 | `7a24069ba7c86b3e0f7bae6e688d2df90e0255af4a89ed7731c6da8644d73fd2` | `61e659e3edd4a0189caf4d42615973431c642d8e` |
| `BrakeSync_全新固件工程模板_V0.2_严格质量门禁.zip` | [V0.2 质量门禁模板](./BrakeSync_firmware_template_V0.2_HOST_ONLY.zip) | 29467 | `4147c0a026f5dec22966389d984b3b81b49ca23083a4355303672ca4646e7a88` | `0ce3fb58667f4077ce8f23ba0b27f2891b6e6467` |

- 通过对话原始字节编码传递后使用 GitHub Git Blob API 创建，**未重新打包**；文件 Git Blob SHA-1 已和本地原文件计算一致。
- V0.1 包含 11 个文件；V0.2 压缩包包含 22 个非目录文件及目录项；均**不包含供应商完整 SDK**。
- V0.2 部分原始 ZIP 条目路径采用非预期编码显示，这是**归档原件**的历史问题，若未来解压使用需单独修正并生成新版本，不可改动本原件。
- GCC 14.2 Host Debug/Release 与 Clang 17 Host 测试日志另存于 [交接证据目录](../evidence/)。
- 本批代码不包含可直接用于云途 MCU 的已验证官方 Target 项目；MISRA 专业检查、Target 编译、烧录、实际外设功能、安全认证均**未确认完成**。

## 交接后续要求

- 当前两 ZIP 的目标是**历史研发资料完整性**，不是将其直接投产。后续 SW-01 应使用真实云途板、已知的 **Keil + ST-Link** 开发路径和经确认的官方 SDK 重新构建目标工程。
- 项目负责人复核下载、内容和验收要求后，在 Issue #1 登记是否 `ACCEPTED_FOR_ARCHIVE`。在此之前状态为 `GITHUB_ARCHIVED / RECEIVER_CONFIRMATION_PENDING`。
- 不将旧 `firmware`、`app` 分支代码纳入本次归档的有效实施基线，不进行代码复用。
