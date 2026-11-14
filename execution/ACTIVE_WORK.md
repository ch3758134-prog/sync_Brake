# BrakeSync｜当前执行工作台：嵌入式软件开发主线（2026-11-14）

> **SW-00 新事实**：负责人已使用 **Keil + ST-Link** 对云途开发板做初步烧录尝试；该路径优先保持，待补 MDK、编译器、ST-Link、Flash Algorithm、SDK 和板卡版本证据，不能据此认定烧录或运行通过。见 [SW-00/SW-01 交接单](../handover/03_SW00_SW01_HANDOFF.md)。

> **受控交付和交接入口**：[handover/00_INDEX.md](../handover/00_INDEX.md)。所有交付物必须有GitHub路径、责任人、签收和验证证据；未入库/受限原件按缺口台账 `OPEN` 追踪。当前管理文件已归档不代表固件代码、Target工程或受限供应商原件已移交。  


> **正式质量规范已批准（2026-11-14）**：[`BS-SW-STD-001 / V1.0`](../baselines/2026-11-14_SW_STD_V1.0/APPROVAL.md) 为强制软件质量基线。当前新源码模板仅 Host 级验证；SW-00 → SW-01 顺序已获认可，但具体固件分支/文件修改、真实功率执行均未授权。 
>

> **质量要求新指令（2026-11-14）**：负责人要求全面严格按照汽车电子安全相关软件工程标准，不再“简化”。[BS-SW-STD-001 V1.0 受控工作稿](./QUALITY/BS-SW-STD-001_V1.0_WORKING.md) 已建立。当前本地新模板 V0.2 的 GCC/Clang Host 严格编译和 CTest 通过；MISRA C:2025 专业检查、Target/硬件及产品安全仍为 `NOT_RUN/BLOCKED`。不修改旧 firmware/app。  
>
> **正式总体基线**：BS-P0P8-BASELINE-003 / V0.3；三版产品顺序为 V1 真实功能测试、V2 LCD 产品交付、V3 App。正式 PCB 未完成，但不阻止开发板驱动和 Host/Target 软件开发。
>
> **当前软件任务工作稿**：[嵌入式软件开发路线与驱动实施任务 V0.1](./SOFTWARE_DEVELOPMENT_EXECUTION_V0.1_WORKING.md)。这是一份执行准备和任务拆解工作稿，未获具体源码修改授权。
>
> **源代码边界**：未向 `firmware` / `app` / `main` 写入任何代码。已在当前对话提供新的 C11 模板 ZIP，并在独立 Host 环境构建测试，**不是 ARM 目标构建**。旧代码不作为新基线。

**未移交完成的具体工作**：[Issue #1 模板原件归档](https://github.com/ch3758134-prog/sync_Brake/issues/1)、[Issue #2 供应商原件受控保管](https://github.com/ch3758134-prog/sync_Brake/issues/2)；负责人/接收人均为项目负责人，AI 在具体权限内执行。二者不等同当前 SW-01 Target 完成。

## 当前先执行的任务

| ID | 具体动作 | 验收 | 状态 |
|---|---|---|---|
| SW-00 | 已知 Keil + ST-Link 初步烧录尝试；核实实板 Rev/MCU、Keil MDK/Arm Compiler、ST-Link 固件、官方 SDK、设备包和下载算法 | 可复现的版本/线束/操作及证据 | PARTIAL |
| SW-01 | 以 **Keil + ST-Link** 为当前首选实际路径重新建立官方 SDK 支持的干净 Target 工程，必要时用 YCT 生成导出 | 可重复 Target Build / Flash Verify / Reset / Debug，各阶段独立证据 | NOT_VERIFIED |
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
