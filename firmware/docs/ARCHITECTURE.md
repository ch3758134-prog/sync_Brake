# BrakeSync 正式固件分层基线 V1.0

负责人于当前对话批准的工程方向：

```text
firmware/
  app/                     启动、初始化与周期/事件调度
  asw/                     制动输入、运动质量、控制、状态、参数和诊断
  bsw/mcal/                官方云途 SDK + 必要 MCU 抽象接口
  bsw/cdd/                 BMA510、CAN IMU、WLT8016 等专用设备
  bsw/services/            时间/可靠存储/系统健康监督
  bsw/board/               BoardCfg 与厂商 YCT 生成的板级配置
  config/                  Board / Variant / Release 独立
  target/                  Keil 等 Target 构建入口
  tests/host/,tests/target/ Host 单测与无功率实板测试
  qa/,docs/                质量、追踪、审查和技术合同
```

接口与依赖：
1. ASW 仅依赖受控输入/服务合同，不引用 MCU 寄存器或云途 SDK。
2. MCAL 的 SDK 原件与自研薄适配物理分离；不重复实现 SDK 中已成熟的外设控制。
3. CDD 实现**真实**传感器/蓝牙模块专用协议，调用稳定总线/时间等服务；不是额外的 MCU 外设驱动。
4. Services 提供跨模块功能；软件 `time_service` 与实际 SysTick 初始化由 `mcal_time` 适配隔开。
5. Board/YCT 提供指定评估板的 clock/pin/startup/linker，不能代替实际产品硬件默认安全状态证明。
6. 当前目标不构建制动功率输出：仅用于非功率 MCU 外设与接口研发，所有危险故障动作另行批准。

技术规则：参考 MISRA C:2025、ISO 26262:2018、ASPICE 4.1；当前无专业 MISRA 报告或功能安全认证，不宣称符合标准认证。
文件名采用 lower_snake_case，公开函数以模块前缀命名，必要中文注释说明接口契约、硬件依据和异常恢复。
