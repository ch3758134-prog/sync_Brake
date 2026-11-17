# BrakeSync 正式固件分层基线 V1.0

负责人于当前对话批准的工程方向：

```text
firmware/
  asw/                     制动业务逻辑及 app/ 初始化/调度入口
  bsw/                     硬件抽象和基础软件
    mcal/                  官方云途 SDK + 必要 MCU 抽象接口
    cdd/                   BMA510、CAN IMU、WLT8016 等专用设备
    services/              时间/可靠存储/系统健康监督
    board/                 BoardCfg、启动/链接和 EVB 无功率自测
  config/                  Board / Variant / Release 独立
  target/                  Keil 等 Target 构建入口
  tests/host/              不链接 MCU 的 Host 单测
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

## 工程组织约定

所有参与 Target 编译的源代码物理归属在 `asw/` 或 `bsw/`，Keil 文件组直接使用 `asw`、`bsw`；不为 IDE 额外创建 `Startup`、`Test` 等另一套分类。厂商 SDK 含芯片系统启动源码，YCT 启动/向量/Scatter 位于 BSW Board；链接脚本由 Target 构建配置引用。Host 测试与构建配置仍独立存放，不参与该分组分类。

应用调度位于 `asw/app/`，目前编入目标的 GPIO 自测位于 `bsw/board/ytm32b1le0/bringup/`，只能用于无制动功率的 EVB 配置，不是正式制动业务模块。
