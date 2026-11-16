# BrakeSync 固件 V0.1｜真实 Keil Target 工程已纳入

## 目标工程

**Keil 可打开文件**：`target/ytm32b1le0/KEIL/BrakeSync.uvprojx`。

源码自带示例所用的启动、链接、Clock/PinMux、PINS/DMA/UART/OSIF/CMSIS 等源。Keil 工程内 89 个相对文件路径已全部核对存在。

- 核验芯片：`YTM32B1LE05`，原示例使用 `YTMicro.YTM32B1LE0_DFP.1.4.0` 和 ArmClang 6.24；设备 Pack / Flash 算法仍须本机安装。
- 新增 `app/main.c`、`app/app.c`、`board/board_support.c`，用于 PTD7 心跳、SW1/SW2 GPIO 中断和 PTD6 指示。
- **由于当前环境没有 Keil 和开发板，新工程 Target Build、烧录及实板结果仍为 `NOT_RUN`。** 原示例成功日志不等于新工程成功。
- **无真实制动功率**；Keil 目标不链接 Host 测试 CAN 假实现，也没有任何制动输出。

操作步骤、许可信息及限制：[Target 工程说明](target/ytm32b1le0/README.md)。

## Host 模块

当前 `bsw/drivers/can_bus.[ch]` 是编码风格与接口 **Host 样板**，尚未实现云途 CAN 控制器。`asw/motion_sample.[ch]` 提供纯 C 数据质量检查样例。

```sh
cmake -S firmware -B build/firmware -DBUILD_TESTING=ON
cmake --build build/firmware
ctest --test-dir build/firmware --output-on-failure
```

Host 测试结果与 Target 实测必须分开记录，不以 Host PASS 证明真实 CAN。严格执行 `BS-SW-STD-001 V1.0`；本分支未宣称已取得 MISRA / ISO 26262 合规评定。
