import '../domain/device.dart';

/// BLE transport / business protocol boundary.
/// No GATT characteristics or AT commands may be guessed here.
abstract class BrakeDeviceRepository {
  Future<List<BrakeDevice>> scan();
  Future<void> connect(String id);
  Future<void> disconnect();
  Stream<BrakeStatus> observeStatus();
  Future<void> updateBrakeLevel(int level);
  Future<void> updateWeightPreset(int preset);
  Future<void> dispose();
}
