import 'package:flutter/material.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:brakesync_app/main.dart';
import 'package:brakesync_app/transport/mock_device_repository.dart';

void main() {
  testWidgets('Mock banner and device scan are visible', (tester) async {
    await tester.pumpWidget(
      BrakeSyncApp(repository: MockBrakeDeviceRepository()),
    );
    expect(find.textContaining('模拟模式'), findsOneWidget);
    await tester.tap(find.text('搜索设备'));
    await tester.pumpAndSettle();
    expect(find.textContaining('BrakeSync DEMO'), findsOneWidget);
    expect(find.byType(Slider), findsOneWidget);
  });
}
