# BrakeSync 固件工程模板（V1.0）

当前研发阶段：**YTM32B1LE05 开发板、无制动功率**。此仓库是嵌入式产品的可迭代工程模板，不是已完成的全部驱动或可直接用于车辆的固件。

```text
firmware/
├── source/
│   ├── main.c                  # 唯一程序入口：初始化、选择当前测试
│   ├── ASW/                    # BrakeSync 产品业务
│   │   ├── Algorithm/          # 制动算法归属（当前未实现控制律）
│   │   ├── BrakeControl/       # 制动业务合同（候审）
│   │   ├── BrakeInput/         # 制动输入质量
│   │   ├── Motion/             # 运动信号检查
│   │   ├── Diagnostics/        # 产品故障处理
│   │   └── SystemState/        # 运行资格
│   ├── BSW/
│   │   ├── MCAL/               # MCU 外设接口（多数为候审接口）
│   │   ├── EAL/                # 实际 ECU/PCB 器件与信号接口
│   │   └── Services/           # 时间、调度、事件、健康等共用机制
│   └── Common/                 # 公共数据定义与错误码
│
├── platform/
│   └── ytm32b1le0/
│       ├── sdk/                # 官方 SDK/CMSIS/OSIF（示例附带子集）
│       ├── startup/            # 启动、向量、Scatter
│       ├── config/             # YCT 生成的 MCU 外设/引脚配置
│       ├── system_init.c/h     # 调用官方 Clock/PinMux 初始化
│       └── system_time.c       # 先前 OSIF 毫秒计时适配（保留兼容）
│
├── config/                     # Board / Variant / Release 三种配置
├── tests/
│   ├── host/                   # PC 单元/模块测试
│   └── target/                 # MCU 实测函数及旧 EVB GPIO 实验
├── target/
│   └── keil/                   # Keil uVision 工程
└── tools/                     # 源文件引用和原始 SDK 完整性检查
```

打开 `firmware/target/keil/BrakeSync.uvprojx`。Keil 的自研代码、官方 SDK/CMSIS、YCT、Startup 和测试源文件分组独立；原 SDK 内部目录不变。

**本轮只确定工程模板和源码归属，不决定新 GPIO/CAN/UART/DMA Driver API。** 以前编写的、尚未审批的 `drv_*.h` 是历史候审接口，不代表本次确定的驱动模板；旧 GPIO 示例实现已转入 `tests/target/legacy/`。

测试目标仍默认 `BS_TARGET_TEST_GPIO`，不会连接真实制动功率。Host 编译、Keil 路径检查、真实 ArmClang 编译、烧录和板级验证是不同证据，不能混用。

参考：[工程目录设计](../docs/architecture/FIRMWARE_LAYOUT_V1.md)、[模板验证结果](../docs/verification/FIRMWARE_TEMPLATE_BASELINE_V1.md)。
