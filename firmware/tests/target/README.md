# Target 测试

test_gpio、gpio_bringup、test_timebase 与 evb_app 长期保留；source/main.c 的 BS_TARGET_TEST_EVB_DEMO/GPIO/TIMEBASE 宏控制测试选择。GPIO IRQ 所有者在 platform/ytm32b1le0/board_port。EVB_NO_BRAKE_POWER，Target 实测未执行。
