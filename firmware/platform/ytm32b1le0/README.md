# YTM32B1LE0 MCU Platform

sdk/ 为官方示例携带的 SDK、CMSIS、Device、OSIF 子集；内部结构和字节不改。
config/ 为 YCT 生成配置及原 .yct；startup/ 为启动/Scatter。
board_port/ 是自研 EVB GPIO/IRQ/Board 初始化，mcal_port/ 为自研时间适配。**与官方 SDK 分离。**
