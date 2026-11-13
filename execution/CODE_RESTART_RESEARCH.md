# BrakeSync｜软件代码重新研究（初审 V0.1）

> **状态：研究记录 / 非批准的实现架构**。2026-11-13 用户要求旧代码资产退出代码基线；新代码待本对话审阅。只在 management 记录本次方向调整，不触碰 firmware、app 或 main，不删除历史代码。

## 已核实代码事实

- 固件分支 `d1f38b250f2c229aeb122985dc0520b969fe971d`：`firmware/CMakeLists.txt` 目前构建 Host 静态核心、`test_bma510`、`test_driver`；`BrakeSystem.c` 返回 INHIBITED，`Can.c`、`Uart.c` 等主要是 BsPort 回调层。它不是已核实 Target 工程。
- App 分支 `5b911b5b57352fb1c8d0017faeb4b00ace078eb0`：Flutter MockDeviceRepository、模拟首页与 widget test；没有实际 BLE GATT/设备联调证据。原 Mock 三档不得成为产品制动档依据。
- 用户历史 `BrakeSync_LE05_Bringup_FixThumb.zip`：YCT 生成的 CMake 工程，`SDK_VER 1_5_0`，`SDK_DIR` 绑定旧 PC 地址。包含启动、向量、中断、Linker、时钟/GPIO SDK 片段；`app/main.c` 的 Board_Init 为空，`board/pin_mux.h` 中 `NUM_OF_CONFIGURED_PINS0 (0U)`，`board/clock_config.c` 中外设时钟列表 `count=0`；该文件不能当作当前已验证的板级功能工程。
- 已核对的硬件依据：`YTM32B1LE0-EVB-LQFP64` 板型；RevC 原理图与 RevA 参考手册可能存在版本差异；PCB 实物修订、蓝牙/加速度模块的实际型号和电平仍需核实。
- 云途官方 YCT 产品说明支持基于器件生成 PIN/Clock/外设配置及 IDE 工程，适合作为新 Target 工程起点。参见 `https://www.ytmicro.com/news-details-37.php`。

## 对过去代码的处理

**停止引用为实现基线**：BsPort 集中式回调和 CDD 代码、Flutter Mock 页面、先前 FW-01 CAN Host 补测试执行包。旧代码可以只读用于了解失败经验、对比已有测试或业务概念，不能直接复制成新工程已批准实现。

**保留**：P0-P8 经批准产品目标/需求与安全边界；云途 EVB 型号、原理图、厂商 SDK；代码风格、可读性、ASW/BSW 责任边界和 Variant/Board/Release 独立配置理念。

**没有发生**：删除代码、覆盖分支、目标编译、烧录、开发板实验、模块实测、真实制动执行。

## 推荐重新研究顺序（候选，非开始实施指令）

1. **SW-RE-01 — 工具链与最小 Target**：拿精确 YCT 项目和完整官方 `YTM32B1LE0` SDK，校核设备型号/启动/时钟/向量/PinMux/链接；写出 CMake 构建与可复现流程。首次 Target build/烧录记录必须实测。
2. **SW-RE-02 — 轻量 ASW/BSW 架构合同**：从需求和约束出发，而非从旧类/文件名逆向设计。确认 Board/驱动/设备解析/服务/ASW 职责和接口，优先 C 静态、确定性、官方 SDK 复用，拒绝多重无效包装。
3. **SW-RE-03 — 传感器与蓝牙链路**：先核实实物模块、供电、接口/电平、资料和接线；针对确认的 CAN/UART/I2C 驱动与原始数据测试，绝不虚构 AT/GATT/CAN 帧。
4. **SW-RE-04 — 共用业务与 Host 测试**：信号质量、时间戳、诊断、状态、参数、逻辑控制请求等完全可独立于最终 PCB 设计/测试，输出端保持禁止。
5. **SW-RE-05 — App 重新评价**：以确认后的 BLE-ICD 和产品功能为依据，决定 Flutter 工程如何建立；不保留 Mock 结构作为强制设计前提。

## 研究第一份交付应回答

目标代码究竟依赖哪些官方 SDK 文件、BoardCfg 需要什么配置、Main 最小职责、哪些业务模块纯 Host 可验证、故障禁能边界在哪里、谁拥有蓝牙/加速度模块协议解释权、哪些参数仍 TBD、Codex 第一笔代码写入的范围和验收方法。

**提醒**：本研究仅替换旧实现起点，不因代码重启撤销已批准的产品需求或整套项目管理方案。新方案应在本对话展示，负责人批准后再授权开发分支实现。
