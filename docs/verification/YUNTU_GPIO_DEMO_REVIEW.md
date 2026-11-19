# 云途 GPIO 原始示例与 BrakeSync 驱动的区分

## 1. 本次审查来源

用户提供的 `Gpio_Interrupt_Demo.zip`，原始入口 `Gpio_Interrupt_Demo/app/main.c`，原 YCT `board/pin_mux.c`，官方 PINS API `platform/drivers/inc/pins_driver.h` 与实现 `platform/drivers/src/pins/pins_driver.c`。当前 BrakeSync 迁移后的对应 SDK/配置在 `firmware/platform/ytm32b1le0/{sdk,config}/`。

原始 ZIP 是用于验证 GPIO 中断的示例，不是完整芯片 SDK 或可直接复用的产品主控软件。**官方示例代码与厂商 SDK 驱动源码不是同一个概念。**

## 2. 原始 Demo main.c 的实际逻辑

- `Board_Init()` 调用 `CLOCK_SYS_Init/UpdateConfiguration` 和 `PINS_DRV_Init`。
- 定义 `SW1=PTE0`、`SW2=PTE1`、`LED1=PTD7`、`LED2=PTD6`；`TOGGLE_PIN_FOR_TEST()` 实际翻转 **PTE6** 输出。
- `GPIO_IRQHandler()` 原样在 Demo 的 main.c 中，进入中断后直接翻转 LED，并对中断次数计数。
- 在 `while (gpioIrqCount < TEST_COUNT)` 中翻转 PTE6 并延时；这是一种**需要引脚短接路径的例程自测**。未接短接线或未触发足够中断时，会卡在该循环，**不能直接拿来作为产品主循环**。
- 原示例实际宏使用 PTE6，但循环内备注写过 `Short PTC1 ...`，与宏的引脚不一致；不能仅凭示例注释确定接线。

## 3. 本项目保留和替换了什么

| 内容 | 原示例 | BrakeSync 当前 |
|---|---|---|
| 厂商 `pins_driver.c/h`、硬件访问和寄存器定义 | 官方 SDK 源文件 | **原样保留**在 platform/sdk，不拷贝重写 |
| CMSIS / Device Headers | 官方依赖 | **原样保留**，Keil 显示为 Vendor/CMSIS 和 Vendor/Device |
| Clock/PinMux 等 YCT | 原生成文件 | **原样保留**在 platform/config，当前仅适用 EVB |
| GPIO 中断处理 | Demo `main.c` 中直接处理 | 自研 `board_port/drv_gpio_ytm32b1le0.c` 唯一处理器，测试不占用硬件向量 |
| GPIO 访问 | 示例直接调用 `PINS_DRV` | 自研 BoardIO API，通过 `PINS_DRV_*` 操作已知 EVB 资源 |
| 按键/LED 测试 | 中断内切换 LED、PTE6 跳线自测 | `tests/target/gpio_bringup.c` 和 `test_gpio.c` 从主循环消费事件，不要求 PTE6 自测连线 |
| 正式产品 ASW | 示例无制动业务 | ASW 独立；同步制动控制律仍未实现 |

自研 Port 通过官方 SDK 的 `PINS_DRV_ReadPin/WritePin/SetPinIntSel/GetPortIntFlag/ClearPinIntFlagCmd`，这些符号已在原 SDK 头文件和实现中找到。**没有通过 ArmClang 目标编译或 GPIO 实板测试验证目前的适配电气行为。**

## 4. 仍需实测的实际问题

- SW1/SW2 的物理开关与上拉/下拉连接、电平极性和上升沿对应按下还是释放；当前 PinMux 中内部上拉未启用。
- PTD7/PTD6 对应的 LED 真实亮灭极性，GPIO 读回是端口采样值还是要另行检查输出锁存值。
- GPIO IRQ 是否正确进入，事件累加和清标志顺序，抖动、丢事件和 NVIC 优先级。
- MCU 目标 Keil Rebuild / 下载 / 断点 / 实际板卡测量。

本次审查是**源码核对**，不是板级 PASS。当前开发板不接真实制动功率。
