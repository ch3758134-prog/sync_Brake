# YTM32B1LE05 EVB｜BrakeSync Keil Target V0.1

## 直接打开工程

在 Windows 上拉取 `firmware` 分支后，用 Keil MDK 打开：

`firmware/target/ytm32b1le0/KEIL/BrakeSync.uvprojx`

该工程基于负责人提供的云途 `Gpio_Interrupt_Demo.zip` 官方示例，原项目历史构建日志显示 Keil uVision 5.43.1 / ArmClang 6.24 编译 **0 errors / 0 warnings**。**该日志来自原示例，不能作为本次改造工程已编译的证据。**

项目的所有 C/头文件、启动、散装配置、CMSIS、PINS、Clock、DMA、UART、OSIF 源均在 Git 仓库内，工程引用仅使用相对路径；**不依赖原作者电脑上的源代码路径**。

### 编译/烧录仍需本机安装的工具

- Keil MDK，具备 Arm Compiler 6.24（原始示例构建版本），本地可用的许可证。
- **YTMicro.YTM32B1LE0_DFP 1.4.0**：必须提供 YTM32B1LE05 设备描述和 `YTM32B1LE0_Main.FLM` Flash 下载算法；无法通过 Git 仓库代替 DFP 安装。
- 对应开发板的 SWD 下载器与驱动。原上传文件的 `.uvoptx` 保存了 **ST-Link** 调试器选择；实际连接、Flash Algorithm 必须在本机 Keil 的 Options for Target → Debug / Utilities 中核对。
- 开发板为 **YTM32B1LE0-EVB-Q64 / MCU YTM32B1LE05**；如实物并非该型号/管脚，不应直接烧录。

### 当前上板可观察功能

- **PTD7**：LED 心跳，主循环每隔 500 ms 翻转一次。无需 GPIO 自测短接线。
- **PTE0 / PTE1**：使用官方生成的按键上升沿 GPIO 中断配置。
- **PTD6**：任一按键中断事件触发一次翻转。
- `board_support_get_button_irq_count()`：可在 Keil Watch 窗口观察的按键中断次数。
- 板级初始化沿用官方 Clock/PinMux SDK 生成文件，失败时进入非运行循环等待调试；不会启动制动功能。

注意：GPIO 引脚和 LED/按钮极性来自**上传官方示例**的 `pin_mux.c`，不应外推到最终产品 PCB。按键引脚需按实际 EVB 配线核对，机械抖动可能产生多次中断。

### Keil 操作

1. 打开上面的 `BrakeSync.uvprojx`，确认 Target 为 `BrakeSync`、Device 为 `YTM32B1LE05`。
2. 检查 Pack Installer 中 `YTMicro::YTM32B1LE0_DFP@1.4.0` 已安装；找不到 Flash Algorithm 时不要强行下载。
3. 执行 Rebuild（F7），确认 **0 errors**，产生 `KEIL/Objects/BrakeSync.axf` / `BrakeSync.hex`。记下编译警告及资源占用。
4. 检查 Debug / Utilities 使用**你电脑实际连接的下载器**与 `YTM32B1LE0_Main.FLM`，进行下载、校验、复位。
5. 观察 PTD7 周期翻转，再单独验证 PTE0/PTE1 → PTD6。记录编译/烧录/复位/LED/IRQ 的独立结果。

## 工程组织

- `firmware/app/main.c`：简短初始化与主循环。
- `firmware/app/app.c`：当前仅运行心跳测试，后续接入调度；不含制动算法。
- `board/board_support.[ch]`：手写的板级适配和 GPIO 中断处理。
- `board/clock_config.*`、`board/pin_mux.*`：官方生成配置，未来 YCT 再生成时按版本审查差异。
- `platform/`、`CMSIS/`、`rtos/`：原包中实际包含的第三方源码；第三方许可和来源登记见 `THIRD_PARTY.md`。
- `KEIL/BrakeSync.uvprojx`：由原 `.uvprojx` 变更名称、app 路径和新增文件得到，启动/链接/外设源与原项目一致。
- `KEIL/BrakeSync.uvoptx`：从示例导入的 ST-Link 调试选项，仅为初始建议；Keil 打开后可能因本机调试器自动调整。
- `Gpio_Interrupt_Demo.yct`：保留上传示例的 YCT 生成配置供审查；不要用 YCT 直接覆盖已审阅的应用与板级代码。

## 不等于已经验证

当前 ChatGPT 环境**没有 Keil/ArmClang 6.24，也没有开发板**，所以本轮只能核对项目路径及 C 语法，无法宣称新项目 Target Build/烧录 PASS。CAN 驱动仍为 Host 样板，未链接 Keil Target。真实制动功率控制始终 BLOCKED。
