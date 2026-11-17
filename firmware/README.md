# BrakeSync 固件｜ASW / BSW / MCAL / CDD / Services / Board

本次按负责人批准的分层组织源树、Keil 文件引用和 SDK 路径。Keil 只显示 ASW / BSW，两者与 Target 源码实际顶层目录对应。**不是重命名 Demo 就视为产品固件**。

## 当前可运行范围

当前 Keil Target 仍是 **YTM32B1LE05 EVB 的无制动功率 Bring-up**，包含：
- MCU 官方 GPIO 示例 SDK、CMSIS、OSIF、启动、链接和 YCT 配置。
- Board 初始化、MCAL 毫秒时基薄适配、Services 时间服务。
- 独立 Target 测试：LED PTD7 心跳及 PTE0/PTE1 按键中断、PTD6 反馈。
- Host 单测：运动采样、时间服务以及已归档的 CAN 接口样板（后者不属于正式 MCAL 驱动）。

```text
firmware/asw/app/
firmware/asw/
firmware/bsw/mcal/{include,ytm32b1le0/sdk}/
firmware/bsw/cdd/
firmware/bsw/services/
firmware/bsw/board/ytm32b1le0/
firmware/config/
firmware/target/ytm32b1le0/KEIL/BrakeSync.uvprojx
firmware/tests/host/
firmware/bsw/board/ytm32b1le0/bringup/
firmware/{qa,docs}/
```

## Keil 本机验收

1. 在 `firmware` 分支打开 `firmware/target/ytm32b1le0/KEIL/BrakeSync.uvprojx`。
2. 核对 YTM32B1LE05、ArmClang 6.24、YTMicro.YTM32B1LE0_DFP 1.4.0 和真实下载器/Flash 算法。
3. 先 Rebuild 并保存完整 Build Output；编译通过后负责人再下载、复位、验证 GPIO。
4. 本仓库 CI 只执行 Host 测试和 Keil 文件/链接元数据检查；**没有实际 Keil 编译/烧录能力**。

本次原官方 GPIO 示例中的 SDK 仅为子集，没有完整 CAN、Flash、WDG 等全部驱动。未验证的外设不加模拟 Target 实现。

## 设计/质量

[批准的架构边界](docs/ARCHITECTURE.md) · [研发及验证状态](docs/STATUS.md) · [Keil 工程说明](target/ytm32b1le0/README.md)

产品制动 ASW、功率硬件、ASIL/功能安全评价尚未完成；当前不允许接任何实际拖挂制动功率链。
