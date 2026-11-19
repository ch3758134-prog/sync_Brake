# BSW Board

`ytm32b1le0/board_support.[ch]` 是开发板初始化代码，`ytm32b1le0/generated/` 保留原 YCT 的 Clock、PinMux、GPIO IRQ、DMA/UART 及 sdk_project_config 等板级配置文件与原 YCT 工程。

芯片 `startup.S`、`vector.S`、RAM 初始化和链接脚本放在源码根目录 `startup/ytm32b1le0/`，不属于 BSW Board 驱动；GPIO EVB 自测在 `tests/target/`，不混入产品板级驱动。

最终 PCB/制动功率安全条件须另行确认，不能从 EVB 示例外推。
