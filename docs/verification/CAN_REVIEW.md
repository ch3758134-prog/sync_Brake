# BrakeSync｜CAN 驱动编码风格审阅样板 V0.1

状态：设计样板 / Host 功能检查通过 / Target SDK 适配未实现 / 非 MISRA 合规证明。

## 为什么提供这几个文件

- `can_bus.h`：业务与设备层使用的稳定 CAN 帧/状态接口；首轮仅经典 CAN **数据帧**（8 字节），尚未决定真实传感器协议。
- `can_bus.c`：有限接收队列、状态、参数检查、统计、bus-off 处理；不出现具体 MCU 寄存器。
- `can_bus_internal.h`：仅给真实 MCU SDK 的 CAN IRQ/回调使用，不给 ASW 调用。
- `can_bus_board.h`：**唯一** YTM32 实现必须满足的最小函数合同；实际 SDK 工程可将这几个函数写在一份板级 `.c` 中，不引入更多 HAL/MCAL 空壳。
- `test_can_bus.c`：Host 假实现和用例；正式 Target **严禁链接**这个文件。

此目录刻意没有 `can_bus_board_ytm32.c`，因为完整对应版本云途 SDK 中的 CAN 配置、回调事件、发送完成和错误恢复语义尚未核验；捏造这样的文件反而是隐患。用户已有 LED 烧录实测，本样板不重新安排 LED 试验。

## 软件收发边界

```text
设备驱动(can_accel) --can_bus_send/read--> can_bus
                                                   |
                                                  can_board_*  （真实 YTM32 SDK 适配）
                                                   |
                                            官方 CAN PAL / FlexCAN
                                                   |
                                                CAN0 IRQ
                                                   |
                                        can_bus_on_rx_isr()
                                        can_bus_on_tx_done_isr()
                                        can_bus_on_bus_off_isr()
```

RX：CAN SDK 接收成功回调须在 SDK 规定的缓冲生命周期中构造 `can_frame_t`，填 MCU `received_at_ms`，放进 8 帧有界队列，并按 SDK 文档正确重装下次接收；协议解码只在主循环执行。

TX：`can_bus_send` 返回 OK 只表示驱动已接受非阻塞发送请求，**不是**总线发送完成，更不证明传感器/对端收到。

故障：bus-off 后清空旧队列、禁发；硬件恢复由周期性监督模块依据准入条件发起。恢复 READY 后，CAN 传感器数据仍须重新取得新鲜度与质量资格，不能继承故障前有效状态。

## ISR 与主循环并发的重要约束

- 本样板为**单 CAN0、单 ISR 生产者、单主循环消费者**，不是多任务/多核通用队列。
- 板级 `can_board_enter_critical()` / `can_board_exit_critical()` 必须保存并精确恢复中断状态，保护**全部**会访问 CAN 队列、诊断状态的 CAN 中断源。
- 使用这个队列前必须验证：ISR 回调不并发/嵌套地写同一队列；目标编译器重排、内存访问、以及关中断函数具有正确屏障行为。`volatile` 不能单独完成这些保证。
- 在独立的目标工程里需要验证接收回调、接收重装、过载丢帧、并发抢占、bus-off 触发/恢复、错误状态、真实 CAN 收发器和物理总线。
- 示例 `can_bus_recover` 不阻塞；其内板级操作成功的语义必须代表恢复了 CAN 控制器及接收条件。重试间隔和降级策略不能靠该模块自己猜测。

## SDK 证据

云途 YTM32B1LE05 社区实际示例使用 `CAN_Init`, `CAN_InstallEventCallback`, `CAN_ConfigRxBuff`, `CAN_Receive`, `CAN_ConfigTxBuff` 等 CAN PAL 接口：
https://forum.ytmicro.com/topic/490/

这**不足以**验证本项目正在使用的 SDK 版本、完整回调签名、IRQ 清除规则、CAN FD 配置、自动恢复及缓冲行为；接入时应审查目标版 SDK 的源码和 Demo。

## 运行已执行的 Host 检查

```sh
gcc -std=c11 -Wall -Wextra -Werror -Wpedantic -O0 -g \
    can_bus.c test_can_bus.c -o test_can_bus
./test_can_bus
```

`gcc 14.2.0`：此样板 Host 检查已通过。另运行过 AddressSanitizer+UndefinedBehaviorSanitizer，在该 Host 测试集内通过。不代表并发模型、静态分析/MISRA、Target 构建或实物测试通过。

## 仍待审批、不可擅定

1. 真实 CAN 模块是经典 CAN 还是 CAN FD、标准或扩展 ID、波特率、过滤策略、时间戳分辨率。
2. 实际 SDK 源码、回调参数、发送完成事件、重装顺序及 CAN Rx FIFO/mailbox 的限制。
3. 当前队列深度 8 帧的 RAM 与突发负载依据；这是审阅用候选值而非正式需求。
4. 设备协议与安全：真实 CAN 加速度模块 ID、字段编码、单位、轴向、采样时序、超时判据；不允许在设备资料缺失时硬编码。
5. Bus-off 后实际等待时间、恢复次数、故障升级以及系统安全动作，须由系统需求和安全分析确定。
6. `can_bus_board` 适配实现及 Keil、实际硬件中断/收发测试仍是 `NOT_RUN`。

验收定位：这份样板用于让负责人审阅**命名、C 代码风格、公开接口、错误处理和 SDK 边界**，不是批准已可上车或可烧录的 CAN 驱动。
