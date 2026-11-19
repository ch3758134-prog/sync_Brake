# MCAL｜MCU 抽象层

## 约定

- `ytm32b1le0/sdk/`：原样保存用户提供的云途 GPIO 示例附带的 SDK、CMSIS 与 OSIF 子集。文件搬迁不改厂商源码内容。
- `ytm32b1le0/mcal_time.c`：BrakeSync 自研适配，仅负责将实际 OSIF 毫秒时基接到稳定接口。
- `include/`：项目需要的稳定 MCU API 合同，按功能逐步增加，避免 SDK 函数同名套壳。
- 新目标的 SDK 供应商版本和许可必须另行核实；**现有材料不是完整云途 SDK**：还缺真正的 CAN 等外设驱动。
- 不将此 MCAL 文件夹称作已通过 AUTOSAR MCAL 一致性或安全资格化的组件。

厂商文件应可对照上游哈希；不得为了编译通过擅自修改底层寄存器实现。

- 厂商 SDK `device_registers.h` 使用相对 SDK 根的 `YTM32B1LE0/startup/system_YTM32B1LE0.h`，因此 `platform/devices/YTM32B1LE0/startup/system_YTM32B1LE0.[ch]` 必须保持原厂路径；不要仅因它名为 startup 就搬出 SDK。
