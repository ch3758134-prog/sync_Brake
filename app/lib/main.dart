import 'package:flutter/material.dart';
import 'domain/device.dart';
import 'transport/device_repository.dart';
import 'transport/mock_device_repository.dart';

void main() {
  runApp(BrakeSyncApp(repository: MockBrakeDeviceRepository()));
}

class BrakeSyncApp extends StatelessWidget {
  final BrakeDeviceRepository repository;
  const BrakeSyncApp({super.key, required this.repository});

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'BrakeSync',
      theme: ThemeData(colorSchemeSeed: Colors.teal, useMaterial3: true),
      home: BrakeSyncHome(repository: repository),
    );
  }
}

class BrakeSyncHome extends StatefulWidget {
  final BrakeDeviceRepository repository;
  const BrakeSyncHome({super.key, required this.repository});

  @override
  State<BrakeSyncHome> createState() => _BrakeSyncHomeState();
}

class _BrakeSyncHomeState extends State<BrakeSyncHome> {
  BrakeConnectionState _link = BrakeConnectionState.disconnected;
  List<BrakeDevice> _devices = [];
  BrakeStatus? _status;
  String? _message;
  StreamSubscription<BrakeStatus>? _subscription;

  @override
  void initState() {
    super.initState();
    _subscription = widget.repository.observeStatus().listen((status) {
      if (mounted) setState(() => _status = status);
    });
  }

  Future<void> _scan() async {
    setState(() { _link = BrakeConnectionState.scanning; _message = null; });
    try {
      final devices = await widget.repository.scan();
      if (mounted) setState(() {
        _devices = devices;
        _link = BrakeConnectionState.disconnected;
      });
    } catch (error) {
      if (mounted) setState(() {
        _link = BrakeConnectionState.error;
        _message = '扫描失败：$error';
      });
    }
  }

  Future<void> _connect(String id) async {
    try {
      await widget.repository.connect(id);
      if (mounted) setState(() {
        _link = BrakeConnectionState.connected;
        _message = null;
      });
    } catch (error) {
      if (mounted) setState(() {
        _link = BrakeConnectionState.error;
        _message = '连接失败：$error';
      });
    }
  }

  Future<void> _changeLevel(int value) async {
    try {
      await widget.repository.updateBrakeLevel(value);
    } catch (error) {
      if (mounted) setState(() => _message = '设置未提交：$error');
    }
  }

  Future<void> _changeWeight(int value) async {
    try {
      await widget.repository.updateWeightPreset(value);
    } catch (error) {
      if (mounted) setState(() => _message = '设置未提交：$error');
    }
  }

  @override
  void dispose() {
    _subscription?.cancel();
    widget.repository.dispose();
    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    final connected = _link == BrakeConnectionState.connected;
    final s = _status;
    return Scaffold(
      appBar: AppBar(title: const Text('BrakeSync')),
      body: ListView(
        padding: const EdgeInsets.all(16),
        children: [
          const Card(child: Padding(
            padding: EdgeInsets.all(12),
            child: Text('模拟模式 · 不连接真实蓝牙，也不发送制动指令',
              style: TextStyle(fontWeight: FontWeight.bold)),
          )),
          Text('连接状态：${_link.name}'),
          const SizedBox(height: 8),
          FilledButton(
            onPressed: _link == BrakeConnectionState.scanning ? null : _scan,
            child: const Text('搜索设备'),
          ),
          for (final dev in _devices)
            ListTile(
              title: Text(dev.displayName),
              subtitle: Text(dev.id),
              trailing: TextButton(
                onPressed: connected ? null : () => _connect(dev.id),
                child: const Text('连接'),
              ),
            ),
          const Divider(height: 36),
          Text('系统状态：${s == null ? '未获取' : s.hasFault ? '故障' : '正常（模拟）'}'),
          Text('制动状态：${s == null ? '未获取' : s.braking ? '制动中' : '未制动'}'),
          const SizedBox(height: 18),
          Text('制动力等级：${s?.brakeLevel ?? '-'}'),
          Slider(
            min: 1, max: 10, divisions: 9,
            value: (s?.brakeLevel ?? 5).toDouble(),
            onChanged: !connected || s == null ? null
              : (v) => _changeLevel(v.round()),
          ),
          const SizedBox(height: 10),
          Text('重量档：${s?.weightPreset ?? '-'}'),
          for (final preset in [1, 2, 3])
            RadioListTile<int>(
              title: Text('档位 $preset（占位名称）'),
              value: preset,
              groupValue: s?.weightPreset,
              onChanged: !connected ? null
                : (v) { if (v != null) _changeWeight(v); },
            ),
          if (_message != null) Text(_message!, style: const TextStyle(color: Colors.red)),
          const SizedBox(height: 20),
          const Text('此界面仅是结构及交互示例。真正车辆参数必须经 MCU 读取、鉴权和回执确认。'),
        ],
      ),
    );
  }
}
