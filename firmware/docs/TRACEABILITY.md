# 需求/实现/验证矩阵（初始基线）

| ID | 项目 | 状态 | 证据/缺口 |
|---|---|---|---|
| SW-DRV-001 | GPIO 统一读写接口 | 源码接口 | 目标引脚/SDK 未绑定 |
| SW-DRV-002 | CAN Classic 帧边界及转发 | 源码接口 | CAN 收发/错误恢复未做 Target |
| SW-DRV-003 | 计时服务 | 源码接口 | 时钟源、溢出、时基精度待验证 |
| SW-DRV-004 | UART / I2C 统一接口 | 源码接口 | DMA、IRQ、时序待验证 |
| SW-CDD-001 | BMA510 上电识别、100Hz/±2g、DRDY/XYZ | 实现待审/Host 测试 | 目标板实测缺失 |
| SW-CDD-002 | WLT8016 UART 透传桥接 | 实现待审/Host 测试 | AT 命令与 GATT UUID 均待厂商文档 |
| SAF-001 | 未安全验证时制动输出禁能 | 源码保证无输出调用 | 不是硬件安全验证 |
| APP-INT-001 | 应用层业务协议 | 未定稿 | 需固件/App 双方接口决策 |

术语：源码完成 ≠ Host Build PASS ≠ Target Build PASS ≠ 烧录成功 ≠ 实板功能验证 ≠ 车规安全认证。
