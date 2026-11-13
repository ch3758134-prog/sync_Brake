# 云途 YTM32B1LE0 EVB 资料差异和初次外设接入审计

> **研究状态**：原理图与参考手册已读取；开发板实物 Revision、连接关系、固件上电实测仍未确认。  
> **设计基线**：`YTM32B1LE0-EVB-LQFP64` 仅为评估与模块验证板，不代表量产 MCU/PCB 已定。

## 核对源

1. `SCH_YTM32B1LE0_EVB_Q64_RevC.pdf`，5 页，RevC，SHA256 `65c9198ac5f31b2f58efb6e8af4205fafb92c7277ed54b68f6063969264f41c6`。
2. `YTM32B1LE0-EVB-LQFP64评估板参考手册.pdf`，16 页，文档 `RM-YTM32B1LE0-Q64-EVB-REVA` V1.0，SHA256 `bfd1af667d38b3fb2f73d469150af3162a877b604a1503fcaa0679616b432c04`。
3. `BST-BMA510-DS000-03.PDF`，Bosch BMA510 芯片数据手册，SHA256 `0eb1c96383b37a31604ad75ea4e389a96390c31426b783915268fe1d8a1e8099`。不能直接当已购模块的规格书。

## 引脚与接口初审（必须以实物及官方 SDK 复核）

| 连接/模块 | 参考手册 RevA 的描述 | RevC 原理图中的实际标注/提示 | 当前结论 |
|---|---|---|---|
| MCU | YTM32B1LE0 LQFP64 | U4 `YTM32B1LE05H0VLHT`，注有 RevC 新器件提示 | **芯片顶标与实板 Revision 待核实** |
| USB-UART | `UART1_TX=PTC9`、`UART1_RX=PTC8` | RevC 原理图存在 USB UART 调试接口和 PTC8/PTC9 引脚 | 先核对 PHY/跳线/复用；不能仅凭这些标签认定 DMA/IRQ 已工作 |
| CAN | `CAN0_RX=PTC6`、`CAN0_TX=PTC7`；STB PTE6 | RevC 有 CAN Interface；具体收发器及供电需看原理图 | CAN 接口存在不等于已购加速度模块必用 CAN |
| 板载 EEPROM I²C | 手册表为 `I2C0_SCL=PTD1`、`I2C0_SDA=PTD0` | RevC 主芯片复用表将 `PTD1`/ `PTD0` 标为 `I2C1_SCL` / `I2C1_SDA` | **接口通道差异待查**，优先以实物芯片匹配的 SDK 头文件/PinMux 与示波器验证 |
| MCU VDD | J2 选择 5V 或 3.3V | RevC 有板级供电与引出 | 外接模块电源和 IO 电平必须单独核对，禁止假设 MCU 的 5V 引脚可直接连接 3.3V 模块 |
| BLE | 并非板载 | 需要外接模块 | 先确认模块准确型号、供电及 UART 参数；不得猜 AT/GATT |
| BMA510 | EVB 不自带 | 外接传感器需独立模块图和连线图 | **BMA510 芯片 ≠ CAN 加速度模块**；实物身份待核实 |

## 下一步实板验证顺序

1. **不接外接模块**：读 PCB revision、MCU 顶标、J1/J2 供电跳线，确认烧录器/SDK 与串口调试链路。
2. **先确定电平与连接**：针对每个外接模块核对原始接线，再单独上电，禁止不核查直接连接。
3. **目标工程构建**：将 SDK/工具链路径、器件型号、链接脚本和精确编译命令记录为 Target Build 证据。
4. **外设单模块实测**：CAN 收发/错误恢复、UART 异步接收、传感器有效数据和故障路径分别验证；保留原始日志。
5. **应用合同与集成**：在 BLE 实际服务/模块固件确认后再跨 App/固件联合冻结命令语义。无真实制动输出准入。

## 不可推断的事情

- RevC 原理图适用性不能自动替代实物 PCB 版本判断。
- 有 Host 单测不能说明片上 I²C0/I²C1、CAN 位时序和 DMA 正确。
- 未确认外接模块不冻结波特率、CAN ID、GATT UUID、传感器单位/坐标及超时门限。
- 尚无最终制动 PCB 或独立执行安全通道，不允许释放功率输出。
