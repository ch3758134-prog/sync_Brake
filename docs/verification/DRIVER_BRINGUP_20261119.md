# BrakeSync MCU驱动推进记录（2026-11-19）

基线 firmware @ b118d27f6990b1f6730b1ded72c95d4cb4ba4b43。仅 EVB_NO_BRAKE_POWER；绝不释放实际制动功率。

- MCAL/GPIO：YTM32B1LE0 通过厂商 PINS SDK 完整配置复用、上下拉、滤波、方向、输出初值，负责唯一 GPIO_IRQHandler。
- EAL/GPIO：PTD7/PTD6、PTE0/PTE1、PTE6 的现有 EVB 电气映射和初始化顺序。系统 init 只初始化 YCT 非 GPIO pinmux，避免重复初始化。
- MCAL/Clock：通过 CLOCK_SYS_GetFreq 读取 CORE/FAST/SLOW/UART1 时钟；初始化继续由厂商 YCT Clock 管理。
- MCAL/UART：UART1，基于 YCT 115200 8N1 的官方 SDK IRQ，256B RX 环形缓冲，192B 异步 TX。DMA_REQ_DISABLED，不宣称 DMA 可用。
- EAL/WLT8016：仅透明字节传输，不宣称 AT/GATT 或 BLE 连接状态已证实。
- EAL/BMA510：Bosch BST-BMA510-DS000-03，CHIP_ID/健康，100Hz ±2g、三轴 burst 读和状态检查；通过回调隔离 I2C 物理总线。现有 SDK 子集无可直接接入 I2C master 实现。
- 测试：GPIO Host EAL fake、BMA510 寄存器模拟、UART/WLT8016 模拟均已在离线代码包中用 GCC -Wall -Wextra -Werror 通过；Keil ArmClang/烧录/实板/外设联调尚未执行。

Keil 中保留旧 GPIO 头文件但排除 legacy/evb_gpio_driver.c 的编译，否则 GPIO_IRQHandler 重复定义。默认仍选择 GPIO 回归；设 BS_TARGET_TEST_SELECTED=3 为 UART1 查看时钟和 RX 统计的安全 Target 测试。

产品级限制：未验证上电输出毛刺、按键电气极性与中断波形、实际 MCU UART1 引脚和 WLT 接线匹配、BMA510 I2C 总线、动态制动算法或真实功率安全。不得将 Host PASS 当作目标芯片验证。
