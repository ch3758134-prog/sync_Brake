# Board / Variant / Release 分离管理

- Board：MCU、板 Rev、真实 Clock/PinMux、收发器和物理线路，当前只有 YTM32B1LE05 EVB 测试配置。
- Variant：V1 功能测试、V2 LCD、V3 App 等功能形态，共用核心制动算法。
- Release：软件版本/精确 Board-Variant 组合/生成工具和构建证据。

这三个维度独立，不把 SDK/板级配置当作产品标定参数。

**当前 Keil Target 只用于 EVB_NO_BRAKE_POWER 调试**，不得连接拖挂制动器。不会创建临时宏允许启用真实功率。
