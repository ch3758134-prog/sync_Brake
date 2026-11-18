# BrakeSync Portable Core — 初次实现状态（2026-11-17）

本提交只实现从 V0.2 候审接口包中能获得明确行为与 Host 测试判据的子集，**不是 40 个头文件全部已实现**。

## 实现及验证范围
- `common/bs_status.h` 和 `asw/bs_control_contract.h`：数据类型候审定义。
- `asw/brake_intent.[ch]`：时间/代次、来源资格、有效释放与制动意图区分。
- `asw/motion_signal.[ch]`：ECU 接收时效、方向已验证标记、质量。
- `asw/diagnostics.[ch]`：定长故障存储、故障锁存保留、无证据禁止清除。
- `app/app_scheduler.[ch]`：单线程、静态数组、无回调的周期任务调度和超期统计。
- `asw/run_qualification.[ch]`：保守的逻辑运行资格检查，不推断实际制动许可。
- `asw/output_authorization.[ch]`：只计算供未来安全审查的逻辑候选条件，不提供任何功率操作。

## 限制
- 不决定制动控制量、重量档映射、故障时物理动作、实际功率许可。
- 不能由 ECU 接收时间推断传感器真实物理采样时刻。参数 `max_age_ms` 只来自外部已批准的时序策略；没有默认值。
- `app_scheduler` 输入为同一启动代次 32-bit 单调毫秒时钟；时间跳变或超过半量程的间隔不在保证范围。SKIP 表示整轮过期不调用该任务，REPORT_ONLY 只返回一次并记录错过数量。
- 新代码目前只在 CMake Host 构建中测试，**没有改 Keil .uvprojx 或现有 Target 调度**。
- V0.2 其它接口（UART/CAN Port、GPIO/Timer MCU 适配、CDD、存储、安全策略与控制算法）继续逐一实现；正式物理执行禁止。

## 如何验证
```sh
cmake -S firmware -B build/firmware -DBUILD_TESTING=ON
cmake --build build/firmware --parallel
ctest --test-dir build/firmware --output-on-failure
```

GCC/Clang Host CI 通过仅说明 Host 编译和单测，不是 Keil Target 编译、烧录和板测。
