# Keil uVision 工程模板

打开本目录的 **`BrakeSync.uvprojx`**。

- 目标设备仍为 `YTM32B1LE05`，沿用原项目工具链及 DFP/下载配置，不自动推断最终产品 MCU。
- Keil Group 分别显示 `ASW`、`BSW`、`Platform`、`Vendor / CMSIS`、`Vendor / SDK Drivers`、`Test / Legacy GPIO`、`Test / Target`。
- `system_init.c/h` 调用来自 `platform/config` 的 Clock/PinMux 配置；`source/main.c` 选定当前测试。
- `BrakeSync.uvoptx` 保留原调试器设置，但移除了原 GPIO 示例遗留的源码浏览缓存；首次打开后由 uVision 重新生成文件树缓存。
- `Objects/` 和 `Listings/` 为编译产物目录，源码仓库不保存编译产物。

## 重要的验证边界

GitHub 的 `firmware-target-metadata` 仅检查文件引用、IncludePath、Scatter 等静态工程信息，不会运行 ArmClang。
负责人应使用对应的芯片 Pack 和 ArmClang 重新执行 **Rebuild All Target Files**，确认编译、链接、下载、运行分别成功。

默认仍为 GPIO 开发板测试；GPIO 实现目前被当作 **legacy 测试代码**，不是已经审批的正式产品驱动。
此模板没有真实制动功率执行程序。
