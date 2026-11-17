# YTM32B1LE05 Startup

原 GPIO 示例中以下厂商源码**内容不变**，为便于检索集中于此目录：

- `startup.S`：复位入口和初始化流程
- `vector.S`、`vector_table_copy.c`：IRQ 向量
- `RamInit0.S`、`RamInit1.c`、`RamInit2.c`：RAM 初始化
- `system_YTM32B1LE0.[ch]`：芯片 SystemInit、时钟基础定义（来自官方 SDK）
- `yt_linker.scf`：Scatter 链接配置

本目录与应用 ASW、MCU MCAL、板级 BoardCfg 分开，仅供特定 Target 使用。对应云途 SDK 及芯片参考手册是修改依据。

注意：上传示例的 `system_YTM32B1LE0.h` 默认定义可能在 SystemInit 阶段关闭 WDG；产品级看门狗与安全策略仍须单独设计、核查及验证。
