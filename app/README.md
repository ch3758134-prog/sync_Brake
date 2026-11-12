# BrakeSync App — 独立平行开发分支

跨平台 Flutter 客户端工程雏形。首期支持：设备发现/连接状态、制动状态显示、制动力等级(1–10)和重量档 UI。**当前使用模拟设备数据，未连接真实蓝牙模块。**

不把这个工程称为已完成蓝牙适配：WLT8016 文档是硬件规格书，没有有效的 AT 指令完整表、UART 默认参数和 BLE GATT UUID，实际 BLE 扫描、服务发现、通知订阅与写入必须核对模块固件版本。

## 运行
```sh
cd app
flutter pub get
flutter run
flutter test
```
若直接从此仓库分支启动，需先在 app 目录执行 `flutter create --platforms=android,ios .` 生成平台模板（不要覆盖已有 lib/ 和 pubspec）。生成的 Android/iOS 工程应随确认的 Flutter 版本纳入后续提交。

安全限制：模拟状态与设置仅用于 UI 开发，不会产生制动请求。不应在 BLE 没有鉴权、完整性校验与确认机制时开放实际制动配置写操作。
