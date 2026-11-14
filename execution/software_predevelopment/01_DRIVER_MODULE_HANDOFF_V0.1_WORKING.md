# BrakeSync｜驱动和软件模块的实施前交接任务包 V0.1

**编号**：`BS-SW-HO-PLAN-001` · **状态**：`WORKING / DESIGN_ONLY` · **日期**：2026-11-14  
**设计依据**：[嵌入式软件研发前设计工作包](./00_SOFTWARE_ARCHITECTURE_PREP_V0.1_WORKING.md)。  
**注意**：本表为未来给 Codex 的任务规格草案；**目前不授权执行、修改源码、安装工具或烧录**。所有未来驱动文件名、SDK API、引脚与 IRQ 必须按实际 YTM32 SDK/板图再次核准。

## 1. 开工先决条件

进入真正 SW-00/SW-01 前应有明确开工授权及目标板、Keil MDK + ST-Link、官方 SDK/插件的真实版本；若缺某项资料，在只读研究中列为 `OPEN/BLOCKED`。当前用户反馈是使用过 Keil + ST-Link，未提交完整编译/下载/运行证据。未来首版 PCB、功率电路未完成不影响独立的无功率开发板驱动实验，但没有独立准入不得开展真实功率制动。

## 2. 精细任务分解（每一条以后独立执行并交接）

| WP | 未来实施内容 | 前置输入与依赖 | 代码产物（目录仅建议） | 正常测试 | 必须设计的异常测试 |
|---|---|---|---|---|---|
| SW-00A | 识别板型/MCU/板图一致性 | 实物焊接型号、PCB Rev、原理图版本 | `docs/BOARD_IDENTITY.md` | 实物/资料核对 | Rev、芯片与PinMux不一致即阻塞 |
| SW-00B | 锁定官方环境与构建来源 | Keil MDK/Arm Compiler/ST-Link/YCT/SDK/Pack | `docs/TOOLCHAIN_LOCK.md` | 工具/文件版本记录 | 版本缺失、Flash算法不符 |
| SW-01A | 全新正式工程与目标构建 | 00A/00B 与SDK示例合法来源 | `platform/...`、`target/keil/...` | 完整编译、ELF/MAP大小 | 缺符号/链接错、不明默认时钟 |
| SW-01B | 烧录/复位/调试验证 | 最小目标工程与安全接线 | `evidence/TARGET_BRINGUP.md` | 下载校验、复位、断点 | 接线错误、连接失败、复位不可控 |
| SW-02A | Clock/Reset 驱动 | 目标时钟树与SDK行为 | `bsw/board/clock*.c` | 频率/复位原因观测 | 时钟源失效、初始化失败 |
| SW-02B | GPIO+PinMux | 原理图/PinMux和测试点 | `bsw/drivers/gpio*.c` | 输入电平、输出、IRQ | 非法引脚、拉电阻、上电状态 |
| SW-02C | Timer/Timebase | 时钟、定时器与IRQ | `bsw/services/time*.c` | 单调性、周期/抖动 | 回绕、时钟故障、取消与重启 |
| SW-03A | UART基本控制和 IRQ | SDK UART/PinMux、IRQ 映射 | `bsw/drivers/uart*.c` | TX/RX、错误标志 | 断线、溢出、错误帧、重入 |
| SW-03B | UART DMA TX | 已有 UART、DMA request/channel依据 | `bsw/drivers/uart_dma*.c` | DMA发送完成与回调 | 失败回滚、并发请求、DMA错误 |
| SW-03C | UART DMA RX | DMA/IRQ行为与缓存约束 | 同模块及相应 `irq` 适配 | 连续流、半包/多包与边界 | 环形缓冲溢出、断包、缓存一致性 |
| SW-03D | CAN控制器与收发器 | MCU CAN能力、实际transceiver、双节点工具 | `bsw/drivers/can*.c` | 收发、滤波、时间戳 | Bus-off、超时、无ACK、恢复 |
| SW-04A | CAN运动设备协议 | 真实模块ID/帧/单位/方向 | `bsw/devices/imu_can*.c` | 原始帧→物理量质量 | 帧丢失、错误ID、错字节序、过期 |
| SW-04B | BMA510设备接口（条件项） | 实物BMA510且手册与总线确认 | `bsw/devices/bma510*.c` | 器件识别、采样与质量 | NACK、复位、方向和量程错误 |
| SW-04C | BLE串口设备协议（前期研究） | 实物BLE模块及AT/透传手册 | `bsw/devices/ble_serial*.c` | 指令/回执/连接状态 | 数据与命令模式错配、超时/非法包 |
| SW-05A | NVM/配置存储 | 闪存EEPROM仿真能力和安全需求 | `bsw/services/storage*.c` | 保存、读取、复位恢复 | 中断/掉电、写失败、校验错 |
| SW-05B | WDG与诊断记录 | MCU WDG、系统故障与资源上限 | `bsw/services/watchdog*.c` | 正常资格、日志 | 程序卡死、持续故障、错误喂狗 |
| SW-05C | Brake input ASW | ICD-01与物理输入确认 | `asw/brake_intent*.c` | 有效输入、变化与时戳 | 意图丢失、噪声、矛盾/过期 |
| SW-05D | Motion ASW | ICD-02、设备质量与时间 | `asw/motion_signal*.c` | 有效测量归一化 | 反向、饱和、通信异常 |
| SW-05E | 状态、参数、诊断 | DD-04/06及可靠存储服务 | `asw/system_state*.c` 等 | 事务生效/保存/读回 | 非法转换、断电、错版本、误清故障 |
| SW-05F | 制动请求/许可接口 | 控制合同、工况与风险分析 | `asw/control_core*.c` | 逻辑请求可追踪 | 资格不合格、限幅、请求过期 |
| SW-06A | V1无功率系统联调 | 所需驱动/设备/ASW模块真实版本 | `tests/target/no_power/...` | 原始输入→逻辑请求→日志 | 任一数据过期、复位、反向、冲突 |
| SW-07A | 实际功率级/执行台架 | **独立硬件设计、安全审查、实体准入** | 以后另建受控任务 | 台架输出及反馈 | 开短路、过温、无法释放（当前 BLOCKED） |

**备注**：CAN与蓝牙设备若并非V1核心闭环要求，可作为并行的设备通信研究，不抢先触发V3 App开发。ADC/PWM/温度等硬件底层只有目标 PCB/执行方案确认后才可细化；不假装现有开发板具有最终产品安全接口。

## 3. 驱动工作包的强制文件

未来每个任务放置/引用下述文档（可集中模板，不能缺字段）：

```text
docs/workpackages/<WP-ID>/
  REQUIREMENTS.md         # 需求与适用版本/硬件
  DESIGN.md               # 模块职责、端口、状态、错误、并发/资源
  SOURCE_BASELINE.md      # SDK、Board、IDE、编译器、依赖版本
  TEST_PLAN.md            # 正常、边界、异常、恢复；可预先设计
  STATIC_ANALYSIS.md      # 专业检查工具与覆盖率、规则/偏离
  TEST_EVIDENCE.md        # 实际命令/日志/测量（未运行则NOT_RUN）
  REVIEW.md               # 评审记录与遗留问题
  HANDOVER.md             # Git SHA、变更清单、负责人、验收与签收
```

## 4. 完成的正确定义

- **DESIGNED**：责任/接口/正常/异常/资源设计经审查，不代表代码存在。
- **IMPLEMENTED**：有精确 Git 代码提交，不代表构建、烧录或功能通过。
- **HOST_PASS**：所记录Host测试通过，不能替代目标板。
- **TARGET_BUILD_PASS**：特定 Board/Compiler/SDK 目标构建通过，不能替代烧录。
- **FLASH_PASS**：下载与校验通过，不能替代运行/中断/DMA真实行为。
- **BOARD_FUNCTION_PASS**：真实板卡正常/异常实验符合设计，需证据。
- **SYSTEM_FUNCTION_PASS**：跨模块真实联调与安全要求范围内实证。
- **PRODUCT_VERIFIED**：指定版本产品需求与安全/法规/交付证据完成，不能由单一模块验收推断。

## 5. 签收与责任链

| 对象 | 责任 |
|---|---|
| 项目负责人 | 唯一真实人类 A/DRI、产品/安全决定与验收；以签收记录而非口头“应该好了”为准 |
| ChatGPT | 工程方案、模块/接口决策、任务范围、设计复核、GitHub受控文档与缺口跟踪 |
| Codex | **只有未来得到具体任务授权后**创建/修改对应文件、运行可用编译/检查并提交报告，不能自主决定架构或危险动作 |
| 供应商/独立安全审查者 | 在需正式合规、安全或外部独立确认时另行落实，不能虚构已存在 |

**当前签收状态**：`SUBMITTED_WORKING_FOR_LATER_REVIEW`。此工作包只定义未来执行条件，不指令现在开始SW-00、SW-01或任何实际功率动作。
