# BrakeSync 源码责任与实现状态（目录重组后）

代码基线：`firmware@3eb98a4786d37818ebca721ddd3cf7b635ad2d34`，后续改动以当前提交 SHA 为准。**这是代码事实表，不是实物功能验收。**

| 部分 | 具体位置 | 来源/责任 | 当前实现情况 |
|---|---|---|---|
| CMSIS-Core | `platform/ytm32b1le0/sdk/CMSIS` | Arm，示例打包 | 原文件完整保存；不是我们实现的芯片驱动 |
| Device 头文件 | `platform/ytm32b1le0/sdk/platform/devices` | 云途 | 定义具体 MCU 寄存器/芯片信息 |
| 云途 PINS/Clock/UART/DMA/IRQ/OSIF | `platform/ytm32b1le0/sdk/` | 云途官方示例随附子集 | SDK API 源码存在；未证明所有接口/特定组合板上可用 |
| YCT 生成配置 | `platform/ytm32b1le0/config` | 云途工具生成，针对当前 EVB | 时钟、复用、UART/DMA 配置仅为生成物，不等于应用已启用 DMA |
| Startup/Scatter | `platform/ytm32b1le0/startup` | 示例原文件 | 不修改；目标链接/烧录仍待验证 |
| Board init/GPIO IRQ/IO 绑定 | `platform/ytm32b1le0/board_port` | **BrakeSync 自研** | GPIO 调用 PINS_DRV_*；IRQ 在自研驱动中唯一实现，实板未验证 |
| 单调时间 Port | `platform/ytm32b1le0/mcal_port` | **BrakeSync 自研** | 利用官方 OSIF 毫秒时基，精度待实测 |
| MCU 通用驱动合同 | `source/BSW/MCAL` | **BrakeSync 自研** | CAN 帧检查已实现；大部分外设仍只有头文件，不能称驱动完成 |
| ECU/PCB 与设备 EAL | `source/BSW/EAL` | **BrakeSync 自研** | BoardIO 有 EVB 功能；CAN IMU/BMA510/BLE 只有合同、缺实协议 .c |
| 通用服务 | `source/BSW/Services` | **BrakeSync 自研** | Time/Scheduler/Health/Buffer/EventLog 存在，存储未实现 |
| 产品业务 ASW | `source/ASW` | **BrakeSync 自研** | 输入质量、运动质量、逻辑资格和故障处理部分实现 |
| 控制算法 | `source/ASW/Algorithm` | **BrakeSync 自研（未来）** | **无同步制动算法正式实现** |
| 开发板测试 | `tests/target` | **BrakeSync 自研及旧 demo 测试重构** | GPIO/Timebase 入口保留；Host 模拟测试不等于板上通过 |

### GPIO 调用的真实路径

`source/main.c` → `tests/target/test_gpio.c` → `tests/target/gpio_bringup.c` → `source/BSW/EAL/BoardIO/drv_gpio.h` → `platform/ytm32b1le0/board_port/drv_gpio_ytm32b1le0.c` → 官方 `sdk/platform/drivers/inc/pins_driver.h` 和 `pins_driver.c` → 芯片。

已对照原 SDK `pins_driver.h/c`：`PINS_DRV_ReadPin`、`PINS_DRV_WritePin`、`PINS_DRV_SetPinIntSel`、`PINS_DRV_GetPortIntFlag`、`PINS_DRV_ClearPinIntFlagCmd` 接口确实存在。**这只证明所调用 API 源码有据，不等于 NVIC、引脚电气、中断时序已被验证。**

### 不照搬的示例内容

官方 GPIO 示例不是产品控制框架：其 YCT 配置只覆盖当前板卡、只带部分 SDK；旧示例的测试流程也不作为 MCU 库直接移植。引脚映射和自研测试、适配都与 SDK 分离，后续不再复制 SDK 自身已有的 GPIO 寄存器操作。

现有板上 GPIO 驱动的 read/write 标志只是可访问权限，不是实际 PinMux/方向配置。已明确改名为 can_read/can_write/supports_irq，以免误读硬件能力；不改变功能。

### 验证边界

只依据同一提交 GitHub Actions 可声明 Host 与工程结构检查；ArmClang Rebuild、烧录、实板中断/GPIO 及真实制动功率均未获得本次验证证据。**EVB_NO_BRAKE_POWER 不能被目录/编译检查绕过。**
