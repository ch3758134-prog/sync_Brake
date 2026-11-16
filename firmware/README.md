# BrakeSync 固件｜新基线（无功率）

## 已建立的内容

- `asw/motion_sample.[ch]`：纯 C 数据质量/时间一致性样板，Host 测试。
- `bsw/drivers/can_bus.[ch]`：先前确认的 CAN 编码风格样板；经典 CAN 单总线、有限 RX 缓冲。
- `bsw/drivers/can_bus_board.h`：真实云途 SDK 适配所需的接口合同，**没有伪造实现**。
- `tests/host/` 与 `.github/workflows/firmware-host.yml`：GCC/Clang Host 构建和测试。
- `target/ytm32b1le0/`：目标 Keil/官方 YCT 工程的准入及集成说明；正式工程文件尚未取得。
- `config/`：Board / Variant / Release 独立管理准则；`qa/` 和 `docs/`：质量及证据。

没有创建没有真实内容的 `brake_control.c`、`uart_dma.c`、`main.c` 或假 SDK 函数。新增模块以实际需求、芯片官方 SDK、原理图和独立验证为依据。

## 运行 Host 测试

```sh
cmake -S firmware -B build/firmware -DBUILD_TESTING=ON
cmake --build build/firmware --parallel
ctest --test-dir build/firmware --output-on-failure
```

此构建 **不包含真实 CAN IRQ/控制器/收发器**；测试中的 `can_board_*` 是 Host 假实现，绝不能链接到 Target。

## 已确认与尚未确认

| 事项 | 当前事实 |
|---|---|
| 开发者此前 LED 编译/烧录/运行 | 已由负责人报告通过，但对应本地 Keil 项目未并入本次 GitHub |
| 本次 Host C 单元测试 | 由 GitHub Actions 生成可检查记录；不等同 Target |
| 实际云途 YCT/Keil 工程文件 | **未取得；不能声称 SW-01 完成** |
| CAN 与 UART DMA 目标驱动 | **NOT_IMPLEMENTED** |
| 实板 CAN 收发 / 中断 / Bus-off | **NOT_RUN** |
| 功率控制及整车验证 | **BLOCKED / NOT_AUTHORIZED** |

批准依据：管理分支 `BS-SW-STD-001 V1.0` 与 P4 已批准系统设计输入；本次仅恢复代码实施基线，不修改管理决策。

下一步：导入**已实测的本地 Keil/SDK 项目**中的真实 `.uvprojx`、生成配置和非保密配套源码；检查精确 YTM32 MCU、SDK 版本及 CAN SDK 回调后实现 `can_bus_board.c`。先 Target Build，再由负责人实板验证。
