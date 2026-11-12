enum BrakeConnectionState { disconnected, scanning, connected, error }

class BrakeDevice {
  final String id;
  final String displayName;
  final BrakeConnectionState state;
  const BrakeDevice(this.id, this.displayName, this.state);
}

class BrakeStatus {
  final bool braking;
  final bool hasFault;
  final int brakeLevel;
  final int weightPreset;
  const BrakeStatus({
    required this.braking,
    required this.hasFault,
    required this.brakeLevel,
    required this.weightPreset,
  });
}
