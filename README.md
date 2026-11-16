# BrakeSync — firmware branch

本分支为 BrakeSync 电子同步制动控制器固件。**当前仅支持无实际制动功率的软件/开发板验证，不可用于车辆。**

- **YTM32B1LE05 Keil Target**：[打开工程说明](firmware/target/ytm32b1le0/README.md)；工程文件：`firmware/target/ytm32b1le0/KEIL/BrakeSync.uvprojx`。
- **Host C 测试**：[固件总入口](firmware/README.md)；与目标硬件代码分开构建。
- **当前证据**：[实施状态](firmware/docs/STATUS.md)。

`main`、`app`、`management` 为平行分支，不在本次固件工程改造范围内。
