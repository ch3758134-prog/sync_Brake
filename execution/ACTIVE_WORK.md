# BrakeSync｜当前执行工作台：嵌入式软件开发主线（2026-11-14）

> **质量要求新指令（2026-11-14）**：负责人要求全面严格按照汽车电子安全相关软件工程标准，不再“简化”。[BS-SW-STD-001 V1.0 受控工作稿](./QUALITY/BS-SW-STD-001_V1.0_WORKING.md) 已建立。当前本地新模板 V0.2 的 GCC/Clang Host 严格编译和 CTest 通过；MISRA C:2025 专业检查、Target/硬件及产品安全仍为 `NOT_RUN/BLOCKED`。不修改旧 firmware/app。  
>
> **正式总体基线**：BS-P0P8-BASELINE-003 / V0.3；三版产品顺序为 V1 真实功能测试、V2 LCD 产品交付、V3 App。正式 PCB 未完成，但不阻止开发板驱动和 Host/Target 软件开发。
>
> **当前软件任务工作稿**：[嵌入式软件开发路线与驱动实施任务 V0.1](./SOFTWARE_DEVELOPMENT_EXECUTION_V0.1_WORKING.md)。这是一份执行准备和任务拆解工作稿，未获具体源码修改授权。
>
> **源代码边界**：未向 `firmware` / `app` / `main` 写入任何代码。已在当前对话提供新的 C11 模板 ZIP，并在独立 Host 环境构建测试，**不是 ARM 目标构建**。旧代码不作为新基线。

## 当前先执行的任务

| ID | 具体动作 | 验收 | 状态 |
|---|---|---|---|
| SW-00 | 核实实板 EVB PCB Rev/MCU 顶标、完整 YCT/SDK、编译器、调试器、实物模块/连线 | 工具和硬件身份的证据清单 | OPEN |
| SW-01 | **全新 YCT 生成 Target 工程**、ARM Build、下载、断点和最小启动 | 可重复 Target Build / Flash / Runtime，各阶段有独立原始证据 | NOT_RUN |
| SW-02 | Clock、GPIO、单调时间基准与异常路径 | 独立驱动文件、测量/波形及正常/错误用例 | NOT_RUN |
| SW-03 | UART DMA（中断、环形/缓冲管理、超时/丢帧） | DMA TX/RX 实测、异常恢复 | NOT_RUN |
| SW-04 | CAN（双节点、接收中断、滤波、Bus-off） | 实测 CAN 收发与错误处理 | NOT_RUN |
| SW-05 | CAN 加速度模块/蓝牙通信的设备层验证 | 实际模块的报文、时间、标识、协议/失效证据 | NOT_RUN |
| SW-06 | ASW 状态/参数/制动控制、无功率联调 | Host + Target 分级证据，不允许偷换成实体制动执行 | NOT_RUN |
| SW-07 | 真实功率台架与车辆测试（后续） | 独立安全/硬件准入及验证方案 | BLOCKED |

## 已产生的客观软件结果

- 新建模板：`BrakeSync_全新固件工程模板_V0.1`，**未从旧固件/App 复制实现**。
- 当前容器 GCC 14.2 / CMake 3.31 / Ninja 的 Host 构建：**PASS**。
- Host CTest：**1/1 套件 PASS**，测试中的逻辑请求无效、物理输出禁止。
- Arm Target 工具链：本环境未检测到 `arm-none-eabi-gcc`；**无 Target 编译、烧录或任何外设实测结果**。

## 协作规则

ChatGPT 担任架构和实施任务定义，Codex 在批准的具体文件范围内执行，负责人验证真实硬件与结果；正常迭代不重走 P0～P8 总体审批。产出 Markdown 而非 HTML。当前开发板**不允许连接真实拖挂制动功率链**。
