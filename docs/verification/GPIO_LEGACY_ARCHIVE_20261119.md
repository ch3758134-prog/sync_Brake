# GPIO IRQ 唯一所有权与旧代码归档

2026-11-19：源代码历史文件 `firmware/tests/target/legacy/evb_gpio_driver.c` 移动至 `docs/architecture/history/evb_gpio_driver.c.legacy.txt`，文本内容保持原样。

原因：新正式 `BSW/MCAL/GPIO/mcal_gpio.c` 定义唯一 `GPIO_IRQHandler()`；Keil 工程不得同时编译旧定义。结构检查遍历 tests/target 下所有 `.c`，因此仅从工程中移除引用会造成检查失败。

原历史 `tests/target/legacy/drv_gpio.h` 与 `board_gpio_ids.h` 暂时保留供旧文档追溯；正式测试入口现调用 EAL GPIO。产品功率输出仍禁止。
