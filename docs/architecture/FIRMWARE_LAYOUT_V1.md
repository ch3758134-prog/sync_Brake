# BrakeSync 嵌入式工程模板 V1.0

**定位：先让 MCU 在真实启动/时钟/外设配置下可靠运行，再驱动 PCB 电路和设备，最后完成同步制动业务。** 借鉴汽车电子的 ASW/BSW、MCAL、EAL、Services 职责边界，但不是 AUTOSAR 产品或 ISO 26262 合规声明。

## 1. 源码结构

见 [固件目录](../../firmware/README.md)。

- `firmware/source/main.c`：唯一程序入口，负责明确初始化顺序与测试选择。没有独立 APP 层。
- `firmware/source/ASW/`：算法、输入质量、状态、诊断及制动控制业务。Algorithm 单列，但真实控制律尚未设计完成。
- `firmware/source/BSW/MCAL/`：未来 MCU 外设合同，目前旧接口待审，不能当作定版 Driver 模板。
- `firmware/source/BSW/EAL/`：实际 PCB 器件与物理信号的接口，不能替代 SDK 芯片外设驱动。
- `firmware/source/BSW/Services/`：公共时间、任务调度、健康、缓冲、事件等机制，不承担制动控制决策。
- `firmware/platform/ytm32b1le0/`：官方 SDK/CMSIS、YCT Config、Startup，另有一份直接组织初始化的 `system_init.c/h` 和保留的 OSIF 时间适配。
- `firmware/tests/{host,target}/`：PC 逻辑测试与真实开发板测试，历史 GPIO 实验实现归属 Target Legacy，不误认为量产 Driver。
- `firmware/target/keil/`：实际目标工程配置；`firmware/config/`：Board/Variant/Release 版本配置，独立于 YCT 芯片配置。
- `docs/`：requirements、architecture、interfaces、safety、verification。

## 2. 为什么不再保留 board_port / mcal_port

它们不是车规架构强制要求的目录。原 `board_port/board_support.c` 的主要内容只是调用 Clock 和 PinMux 初始化，现由 `platform/system_init.c` 承担。
原 `board_port/drv_gpio_ytm32b1le0.c` 使用的是评估板 LED/按钮逻辑 ID，属于尚未批准的**实验测试代码**，已移到 `tests/target/legacy/`。
原 `mcal_port/mcal_time.c` 只是将当前 OSIF 毫秒 API 转接给既有时间服务，保留为 `platform/system_time.c` 以维持当前测试依赖；Timer Driver 模板待后续再设计。

## 3. SDK 与示例的边界

用户提供的 `Gpio_Interrupt_Demo.zip` 主要为当前芯片启动、Clock、PinMux、下载与 SDK 调用提供已知参考，不作为产品 GPIO Driver 的官方模板。
`sdk/` 内 CMSIS、Device、PINS、OSIF 等厂商文件以及 `config/`、`startup/` 迁入后保持原 Blob，不自行重写驱动。此来源是**示例所包含的有限 SDK 子集**，缺少完整 CAN 等实际驱动实现。

## 4. 当前测试与保守边界

`main.c` 默认选 GPIO 测试，既有 GPIO IRQ 实验实现仍是当前测试固件的唯一 IRQ 所有者，后续讨论 Driver 模板时必须处理其归属，不能保留重复 ISR。
宿主机测试通过 ≠ Keil ArmClang 编译通过 ≠ 烧录成功 ≠ 实板功能通过。正式制动输出、故障关断与最终 PCB 尚未获得安全批准，EVB 不能连接真实拖挂制动功率。

## 5. 设计冻结范围

**本版冻结的是目录和 MCU 模板入口，不冻结 GPIO/CAN/UART/DMA、设备驱动的 API/回调/中断/缓冲/异常恢复设计。**
这些要等后续基于真实完整 SDK、芯片手册和产品需求逐项讨论。
