# 官方输入与改动追踪

- 原始输入：`Gpio_Interrupt_Demo.zip`，用户本轮上传。
- 官方示例主体：GPIO interrupt self test，PTE6 输出短接到 PTE0/1 才触发自测中断。
- 上游构建日志：Keil 5.43.1 / ArmClang 6.24，0 errors / 0 warnings；其 `.axf`/`.hex` 是**原始 Demo 产物，均未被作为新版固件提交**。
- 新工程变更：名称 `BrakeSync`；将 main 转移到 `firmware/app/`；引入 `app.c/h` 和手写 `board_support.c/h`；去掉需要短接跳线才能走出循环的自测等待；保留 SDK Clock/PinMux/IRQ 向量和 Scatter 布局。
- 原始 `.yct` 生成配置保存作证据；原始 `.uvoptx` 中的 ST-Link 配置转为 `BrakeSync.uvoptx`。
- 真实 Target Build / 下载 / 烧录 / LED / GPIO IRQ 测试仍由负责人执行并独立登记。

原始 ZIP SHA-256：`79d79af09ed30bc7f5179b10e4d2391dc2bf66a52b2fd7ba2ed2dd422ec5aa1c`。

## 当前路径（迁移记录）

正式 APP 位于 `firmware/app/`；YCT BoardCfg 在 `firmware/bsw/board/ytm32b1le0/generated/`；芯片启动及链接脚本位于 `firmware/startup/ytm32b1le0/`；GPIO 无功率自测位于 `firmware/tests/target/`。以上档案历史事实保留，不应视作当前路径说明。
