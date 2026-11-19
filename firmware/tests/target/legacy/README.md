# 历史 EVB GPIO 实验适配（仅供开发板测试）

这里保留先前编写的：

- `evb_gpio_driver.c`：原 `drv_gpio_ytm32b1le0.c`，含逻辑 ID 映射、事件计数及唯一 `GPIO_IRQHandler`。
- `drv_gpio.h`：原 GPIO 公共接口定义。
- `board_gpio_ids.h`：当前评估板 LED/按钮的逻辑 ID。

这些文件原样迁入，不代表认可接口形式。原 Demo 提供的是 MCU 工程、YCT 配置和 SDK 使用参考，**不能由此推定正式 GPIO 驱动应该这么封装**。

这几个文件在现阶段的 Keil 测试编译中继续参与，避免出现新的中断向量所有权冲突。设计正式 Driver 模板时必须明确如何替换它们，并防止同时存在两个 GPIO IRQ 实现。
