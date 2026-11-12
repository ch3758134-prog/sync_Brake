# BrakeSync firmware — architecture baseline 0.1

这是可审查的固件源码骨架，不是可上车的软件发布件。

- ASW: 制动业务状态与策略（此提交**不释放实际制动输出**）。
- BSW/Driver: MCU 外设语义 API，包括 GPIO、CAN、Timer、UART、I2C；只有函数契约和拒绝缺失驱动的实现，没有虚构云途 SDK API。
- BSW/Cdd: Bosch BMA510 I2C 设备驱动和 WLT8016 UART/BLE 透传设备层。
- board: 云途板卡/SDK 接入点，暂不指定管脚和中断。
- tests/host: 使用真实 CDD 源码测试总线读写、失败路径、无效测量和未绑定端口。
- .github: Host CI 构建验证；不等同 Target Build PASS。

工程状态：Host 目标待 CI 实际运行；Target 没有 SDK 编译/烧录/硬件功能证明。
此前交付的手册为 BMA510 BST-BMA510-DS000-03、WLT8016 F1_15 Rev2.1。
**禁止用本提交作为生产或安全认证依据。**

## Build
```sh
cmake -S firmware -B build/firmware -DBUILD_TESTING=ON
cmake --build build/firmware
ctest --test-dir build/firmware --output-on-failure
```

后续 SDK 适配：只在 `board/` 引入经审核版本的 YTMicro 官方 SDK、时钟、中断、引脚和资源分配。
