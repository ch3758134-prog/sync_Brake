# 实施状态与证据

| 工作包 | 状态 | 证据/说明 |
|---|---|---|
| 新版仓库文件树 | CREATED | 当前 `firmware` 分支 HEAD |
| 代码风格 CAN 样板 | HOST_SAMPLE | `bsw/drivers/can_bus.c`，非 Target 适配 |
| 运动数据质量样板 | HOST_SAMPLE | `asw/motion_sample.c` |
| GCC/Clang Host 构建及测试 | CI_PENDING | 以新提交 GitHub Actions 的实际结论为准 |
| 用户已有 LED 实测 | OWNER_REPORTED | 本地工程未在本次可访问文件中取得 |
| 云途 Keil Target 工程 | NOT_IMPORTED | 缺少原始已实测工程文件 |
| SDK/PinMux/Clock/CAN IRQ 集成 | NOT_IMPLEMENTED | 不能猜测芯片和 SDK |
| MISRA:2025 专业静态分析 | NOT_RUN | 需适用规则与实际工具 |
| CAN 实板双节点和 Bus-off | NOT_RUN | 由负责人实测 |
| 真实制动功率 | BLOCKED | 独立硬件安全审查/台架准入前禁止 |

**本次操作仅修改 GitHub `firmware` 分支。** 原有 Host-only 源码不进入新树，但提交历史保留；`main`、`management`、`app` 无改动。
