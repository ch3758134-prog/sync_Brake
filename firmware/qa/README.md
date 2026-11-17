# 质量与安全准入

参考项目管理分支已批准的 BS-SW-STD-001 V1.0（MISRA C:2025、ISO 26262、ASPICE 等适用工程实践）。

- 每个新模块记录需求、设计、接口、错误/超时/复位和恢复、资源/最坏时序、代码审查、Host/Target 原始证据。
- 保留厂商 SDK、自动生成代码和自研接口的清晰来源边界及变更记录。
- GitHub Host GCC/Clang PASS 不代表新 Keil Target Build / 实板 PASS；metadata 路径核对不是编译。
- 专业 MISRA 检查、代码覆盖、安全分析/ASIL 评定均没有完成，不宣称合规。
- 看门狗、Flash 可靠写入及制动功率安全不得以临时空壳/无条件喂狗替代设计和实测。
