# BrakeSync 固件结构 — Keil 与源码同步（2026-11-17）

真实目录与 Keil 分组保持一致。**SDK/Startup 不迁移或复制**，编译器/散装链接参数不变。

```text
firmware/
  app/                   APP 初始化 + 调度器（现时 Target 保持 EVB 自测）
  asw/                   与 MCU 无关的意图/运动/资格/诊断模块
  common/                公共 C 数据类型与错误码
  bsw/
    drivers/             跨 MCU 驱动 API；CAN 帧校验已实现
    cdd/                 BMA510/CAN IMU/BLE 设备协议接口（仅头文件）
    internal/            UART/CAN MCU Port 的内部接口（仅头文件）
    mcal/                特定 MCU SDK/适配，GPIO EVB 适配已实现
    board/               YCT BoardCfg + EVB 逻辑 IO ID
    services/            当前时钟时间服务；存储/健康后续
  startup/               独立的汇编/向量/Scatter
  tests/                 Host 与 EVB Target 测试
  target/                Keil 工具链工程
```

## 完成状态

- Keil 已编入新 ASW .c、`app_scheduler.c`，但未让它们介入制动实时流程。
- Driver：`drv_gpio.h` 的当前 EVB 读写、IRQ 事件已由 `drv_gpio_ytm32b1le0.c` 实现；`drv_can_frame_validate` 是纯 C 帧合法性检查；CAN/UART 的实际控制器驱动、其他外设均仍未实现。
- CDD：正式接口头文件入库和加入 Keil，可直接翻阅，`imu_can/bma510/ble_serial` **没有 .c 实现、也不能调用**。
- `GPIO_IRQHandler` 从 `tests/target` 转移至真正驱动适配，EVB 自测通过 `drv_gpio` API 读写和取事件，避免一个向量存在两个所有者。
- GPIO EVB ID 只依据当前已归档 YCT 引脚：PTD7/PTD6 为 LED，PTE0/PTE1 为按钮；未来产品板需独立 BoardCfg。
- 现有 SysTick 时间服务未重新实现，`app_run_once` 仍为 EVB 测试运行；不开放功率操作。
- `drv_can_frame_validate` 只检查经典 CAN 帧结构，不代表发送、接收、Bus-off 或真实设备协议可用。
- 各种新增 API 的参数值、DMA 资源和真实通信周期须根据完整 SDK/设备协议/时序需求确定。

## 验证等级

- Host CMake + 新增 CAN 帧测试：请查看 GitHub Actions 的 gcc/clang 结果。
- Keil 源码路径 / 分组 / IncludePath 元数据：`python firmware/tools/check_target_project.py`。
- **真正 Keil ArmClang Rebuild、下载、按键 IRQ/LED 实测仍需负责人在实板确认**。本提交改变 GPIO ISR 所有权，因此尤其要做 GPIO Target 回归。

无真实制动功率接线；没有新增 PWM、执行使能、制动功率接口。
