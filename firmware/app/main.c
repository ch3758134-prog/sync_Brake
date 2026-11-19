#include "app.h"
#include "board_support.h"
#include "bs_status.h"
#include "time_service.h"
#include "test_gpio.h"
#include "test_timebase.h"

/*
 * EVB_NO_BRAKE_POWER 测试选择：仅修改这一行。
 * 0 = 现有 APP 集成入口（仍非正式制动固件）
 * 1 = GPIO / 按键 IRQ 回归
 * 2 = 软件时间基准观测
 */
#define BS_TARGET_TEST_APP       (0)
#define BS_TARGET_TEST_GPIO      (1)
#define BS_TARGET_TEST_TIMEBASE  (2)
#define BS_TARGET_TEST_SELECTED  BS_TARGET_TEST_GPIO

/* Keil Watch 可监视该变量；错误时停在 main 的无限循环。 */
volatile bs_status_t g_target_test_last_status = BS_STATUS_NOT_INITIALIZED;

int main(void)
{
    if (board_support_init() != BOARD_STATUS_OK)
    {
        for (;;)
        {
        }
    }

#if BS_TARGET_TEST_SELECTED == BS_TARGET_TEST_APP
    if (app_init() != APP_STATUS_OK)
    {
        for (;;)
        {
        }
    }
#else
    time_service_init();
#if BS_TARGET_TEST_SELECTED == BS_TARGET_TEST_GPIO
    g_target_test_last_status = test_gpio_init(time_service_now_ms());
#elif BS_TARGET_TEST_SELECTED == BS_TARGET_TEST_TIMEBASE
    g_target_test_last_status = test_timebase_init(time_service_now_ms());
#else
#error "Unsupported BS_TARGET_TEST_SELECTED"
#endif
    if (g_target_test_last_status != BS_STATUS_OK)
    {
        for (;;)
        {
        }
    }
#endif

    for (;;)
    {
#if BS_TARGET_TEST_SELECTED == BS_TARGET_TEST_APP
        if (app_run_once() != APP_STATUS_OK)
        {
            for (;;)
            {
            }
        }
#elif BS_TARGET_TEST_SELECTED == BS_TARGET_TEST_GPIO
        g_target_test_last_status = test_gpio_run_once(time_service_now_ms());
#elif BS_TARGET_TEST_SELECTED == BS_TARGET_TEST_TIMEBASE
        g_target_test_last_status = test_timebase_run_once(time_service_now_ms());
#endif
#if BS_TARGET_TEST_SELECTED != BS_TARGET_TEST_APP
        if (g_target_test_last_status != BS_STATUS_OK)
        {
            for (;;)
            {
            }
        }
#endif
    }
}
