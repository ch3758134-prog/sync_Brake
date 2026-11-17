# EVB Target 无功率测试

`gpio_bringup.c` 是唯一默认纳入当前 Keil 的板上自测：
- PTD7：非阻塞每 500 ms 翻转（基于 OSIF SysTick）。
- PTE0/PTE1：GPIO 按键中断；PTD6 提示灯；Watch 观察按键中断计数。
- 上电执行 Board clock/pin 初始化，然后执行 SDK OSIF(0) 时基初始化。

源自上传 GPIO Demo 的 Clock/PinMux 与 IRQ 依据；**本次 Keil Build、Flash、目标运行、时序测量均未执行**。
若 LED/键位与实际 EVB 硬件不匹配，按原理图与上板证据修正，不将自测状态推广到产品板。
