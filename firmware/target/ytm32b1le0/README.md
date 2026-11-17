# YTM32B1LE05 EVB — Keil

打开：`firmware/target/ytm32b1le0/KEIL/BrakeSync.uvprojx`。

此处只保存工具链和构建选项；真实源码在 `app/`、`asw/`、`bsw/`、`startup/` 和 `tests/`，Keil 直接引用，不重复拷贝。

- MCU：YTM32B1LE05，原演示 Pack YTMicro.YTM32B1LE0_DFP 1.4.0，ArmClang 6.24
- 编译前确认本机 Flash `YTM32B1LE0_Main.FLM` 和实际 SWD 调试器
- 首先 Rebuild All Target Files、保存日志；然后由负责人决定是否进行仅无功率 EVB 烧录验证
- PTD7 心跳、PTE0/PTE1 → PTD6 IRQ 测试结果与本次 Keil 新工程编译均为 NOT_RUN

Keil Group 仅按物理源码目录分组，不承担软件架构职责。

### SDK 头文件回归修复

本次恢复 `system_YTM32B1LE0.[ch]` 到云途 SDK 的 `platform/devices/YTM32B1LE0/startup/` 路径，以满足 `device_registers.h` 的原始 include。Keil 编译器配置、Flash 算法及调试器参数不变。前一次本机编译失败的 21 个错误均引用这个头文件；需再次执行 Rebuild 确认，不将 GitHub 路径检查视为真实编译。
