# BrakeSync 固件目录及分层基线 V1.0

正式工程以源码职责自然组织，而不是为了 Keil 分组搬动代码。

```text
firmware/
├── app/                   程序入口和运行初始化
├── asw/                   制动应用、输入/运动质量、状态和诊断
├── bsw/
│   ├── mcal/              云途官方 SDK 与必要 MCU 适配
│   ├── cdd/               BMA510 / CAN IMU / WLT8016 专用设备
│   ├── services/          通用时基、可靠存储、系统健康
│   └── board/             板级 Clock/PinMux/外设配置
├── startup/ytm32b1le0/   Reset、Vector、RAM init、SystemInit、Scatter
├── tests/
│   ├── host/              不使用 MCU 的单元测试
│   └── target/            开发板无制动功率自测
├── target/ytm32b1le0/    Keil 等编译/下载配置
├── config/                Board / Variant / Release 独立
├── docs/
└── qa/
```

## 分层规则

- ASW 负责制动业务；不引用特定 MCU SDK 或寄存器。APP 执行初始化和调度，不属于驱动层。
- MCAL 保留官方云途 SDK 原件和少量受控适配，不能自称已实现 AUTOSAR 标准 MCAL。
- CDD 负责选定专用外部设备的实际协议与诊断，不能直接拥有制动业务决策权。
- Services 提供独立于 MCU 的公共时间/存储/健康能力；BoardCfg 只负责具体板级器件、Clock/PinMux 和外设引脚配置。
- `startup` 独立保存原芯片复位入口、向量、RAM 初始化、`SystemInit` 和 Scatter；搬迁不修改厂商代码逻辑。
- `tests` 与 `app/asw/bsw` 物理分离；当前 Keil 仍编入 `tests/target/gpio_bringup.c` 供 EVB 自测，不代表产品制动输出。
- Keil `.uvprojx` 只保存工程文件引用，逻辑分组取源码真实相对路径：`app`、`asw`、`bsw/board`、`bsw/mcal`、`bsw/services`、`startup`、`tests/target`。不为任何 IDE 另设源代码目录。

参考项目已批准的 MISRA C:2025 / ISO 26262:2018 / Automotive SPICE 4.1 工程实践。当前尚未执行产品安全合规评定和全部目标实测。
