# 工程质量门禁

沿用 `management/baselines/2026-11-14_SW_STD_V1.0/BS-SW-STD-001_V1.0_批准原文.md`。

- 编译警告、代码审查、单测、异常和资源边界有原始报告。
- MISRA C:2025 专业工具和合规判定 **NOT_RUN / NOT_CLAIMED**；不得凭 GCC/Clang 的 `-Wall` 声称通过 MISRA。
- Host C 构建与 MCU Target 构建分别出具证据，Host 测试替身不得进入 Target。
- 对新的制动逻辑、实际故障动作、CAN 时序和执行电路实施单独需求/安全审查。
- AI 编写并自测不构成独立安全审查。
