# YTM32B1LE05 EVB｜无制动功率 Keil Target

**本 Target 是正式代码组织下的板级 Bring-up 配置，不是已验收的制动控制 ECU。**

直接打开：
`firmware/target/ytm32b1le0/KEIL/BrakeSync.uvprojx`

工程内容与路径：
- MCU 底层 SDK、CMSIS、OSIF：`firmware/bsw/mcal/ytm32b1le0/sdk/`，厂商源码未修改。
- SDK 适配：`firmware/bsw/mcal/ytm32b1le0/mcal_time.c`。
- 板级：`firmware/bsw/board/ytm32b1le0/`，YCT 生成物在 `generated/`。
- 服务：`firmware/bsw/services/time_service.c`。
- 无功率自测：`firmware/bsw/board/ytm32b1le0/bringup/gpio_bringup.c`。
- `asw/app/main.c`：只负责板级初始化、应用初始化和调度入口。

### 本机工具条件
- Keil uVision，原示例记录 MDK 5.43.1 / ArmClang 6.24。
- YTMicro.YTM32B1LE0_DFP 1.4.0，设备 YTM32B1LE05，Flash Algorithm `YTM32B1LE0_Main.FLM`。
- 原选项文件保留 ST-Link 下载器设置，本机实际驱动和 Utilities 需核对。

### 使用
1. Rebuild All Target Files；保存完整 Build Output、警告与资源占用。
2. 编译成功后下载并执行 Flash 校验/复位。
3. PTD7 应每 500 ms 翻转；PTE0/PTE1 按键中断触发 PTD6；观察 `gpio_bringup_get_button_irq_count()`。
4. 遇到不符请保留工程修改前原状态和编译日志，先复核 EVB 真实连接和 SDK 行为。

当前材料可核对 Keil 路径和 MCU C 语法，但不能在此环境运行 Keil ArmClang Build、Flash 或实物测试。原 Demo 历史成功不代表新结构的构建成功。

Keil 左侧仅显示 `asw`、`bsw` 两组，与编入当前 Target 的源码实际目录一致。硬件启动/向量等文件属于 BSW Board，Keil 工程路径保留在 `target/` 只是构建工具配置，不新增一套源码分类。
