# BrakeSync

拖挂式房车电子同步制动控制研发项目。分支 `main`、`firmware`、`app` 和 `management` 并行，互不自动合并。

本分支采用负责人批准的 `ASW / BSW (MCAL / CDD / Services / Board)` 轻量化分层，见 [固件工程](firmware/README.md)。

**当前仅有 YTM32B1LE05 EVB 无制动功率 Target**。SDK 的 CAN/Flash/WDG 等完整驱动尚未具备，不能将目录重建视为汽车功能安全认证通过。
