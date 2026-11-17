# BrakeSync firmware｜研发和验证状态

| 项目 | 状态 | 本次证据 |
|---|---|---|
| GitHub 正式分层目录 | REBUILT | 实际源码归属 app/asw/bsw/startup/tests；Keil 只引用 |
| 云途 GPIO 示例 SDK 子集迁入 MCAL | CODE_PRESENT | 复用旧 Git 对象 SHA，不改厂商源码 |
| YCT/Clock/PinMux 属于 BSW Board，启动/向量/Scatter 独立 | CODE_PRESENT | 官方示例生成物及配置保留 |
| Board/MCAL time adapter/Services | CODE_PRESENT | Keil 已引用，使用官方 OSIF SysTick API |
| LED/按键自测 | CODE_PRESENT | 位于 tests/target，仅作为 EVB 无制动功率自测 |
| Keil 目录和源文件检查 | CI_CHECKED | 检查自然文件夹分组、include、启动和 Scatter，非 ArmClang Target 编译 |
| Host CAN 样板 | HOST_SAMPLE_ONLY | 位于 tests/host/can_sample，不是可执行 MCU CAN 驱动 |
| BMA510 / WLT8016 / CAN IMU | DESIGN_ONLY | CDD 目录职责已建立，真实设备接口未实现 |
| 其他正式 MCU 驱动 | SDK_PARTIAL | 当前仅有示例附带的 SDK 驱动子集，CAN 等仍缺 |
| Host GCC/Clang 编译和测试 | CI_CHECKED | 以同一提交的 Actions 实际状态为准 |
| Keil Target Build / Flash / 实板 | NOT_RUN | 需要本机 Keil 与开发板，用户负责实测 |
| MISRA C:2025 / 功能安全评审 | NOT_RUN | 严格标准工程方法，并非已经符合标准认证 |
| 真实制动功率 | BLOCKED | 无可执行功率驱动，硬件/危险分析未批准 |

禁止把 Host 或结构检查结果当作目标实测通过。

本次新增将现有 `asw/motion_sample.c` 纳入 Keil 编译文件列表，仅增加可浏览/编译的现有模块，不代表已实现制动闭环控制；Target Build 待本机验证。
