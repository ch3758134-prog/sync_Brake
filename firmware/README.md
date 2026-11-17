# BrakeSync Firmware

项目以真实文件夹布局管理源码，Keil 不另造一套源代码架构。

```text
firmware/
  app/                    系统入口
  asw/                    控制业务
  bsw/{mcal,cdd,services,board}/
  startup/ytm32b1le0/     启动、向量、芯片初始化、链接
  tests/{host,target}/    单测与开发板自测
  target/ytm32b1le0/KEIL/ Keil 构建配置
  config/ docs/ qa/
```

当前 YTM32B1LE05 开发板以用户给出的官方 GPIO 示例为可驱动芯片的已知参考，SDK 是示例自带的官方源代码子集，并非完整 CAN 等外设 SDK 软件包。MCAL 保留官方 SDK 的原有内部目录结构，包括 `platform/devices/YTM32B1LE0/startup/system_YTM32B1LE0.[ch]`（厂商内部 `#include` 依赖该路径）；YCT 生成的启动汇编、向量、RAM 初始化和 Scatter 原件集中放在独立 `startup/`，文件内容不改。

打开 `firmware/target/ytm32b1le0/KEIL/BrakeSync.uvprojx`；核对 YTM32B1LE05、ArmClang 6.24、Device Pack 1.4.0、下载器和 Flash Algorithm。本机 Keil 实际编译与烧录不由 GitHub CI 代替。

`tests/target/` 仅包含 EVB 无功率 GPIO 心跳/中断自测；真实制动功率与车辆验证 BLOCKED。

架构见 [ARCHITECTURE](docs/ARCHITECTURE.md)，状态见 [STATUS](docs/STATUS.md)。
