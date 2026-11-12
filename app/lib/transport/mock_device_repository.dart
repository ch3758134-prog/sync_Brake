import 'dart:async';
import '../domain/device.dart';
import 'device_repository.dart';

/// UI test fixture. Never communicates with real brake hardware.
class MockBrakeDeviceRepository implements BrakeDeviceRepository {
  final StreamController<BrakeStatus> _controller =
      StreamController<BrakeStatus>.broadcast();
  int _level = 5;
  int _weight = 1;
  bool _connected = false;

  @override
  Future<List<BrakeDevice>> scan() async => const [
    BrakeDevice('mock-only', 'BrakeSync DEMO (模拟)', BrakeConnectionState.disconnected),
  ];

  @override
  Future<void> connect(String id) async {
    if (id != 'mock-only') throw ArgumentError('Unknown demo device');
    _connected = true;
    _publish();
  }

  @override
  Future<void> disconnect() async {
    _connected = false;
  }

  @override
  Stream<BrakeStatus> observeStatus() => _controller.stream;

  @override
  Future<void> updateBrakeLevel(int level) async {
    if (!_connected || level < 1 || level > 10) {
      throw StateError('Invalid setting or not connected');
    }
    _level = level;
    _publish();
  }

  @override
  Future<void> updateWeightPreset(int preset) async {
    if (!_connected || preset < 1 || preset > 3) {
      throw StateError('Invalid setting or not connected');
    }
    _weight = preset;
    _publish();
  }

  void _publish() {
    if (!_connected || _controller.isClosed) return;
    _controller.add(BrakeStatus(
      braking: false,
      hasFault: false,
      brakeLevel: _level,
      weightPreset: _weight,
    ));
  }

  @override
  Future<void> dispose() => _controller.close();
}
