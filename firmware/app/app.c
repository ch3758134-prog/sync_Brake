#include "app.h"

#include "gpio_bringup.h"
#include "time_service.h"

/* 当前 Target 为 EVB_NO_BRAKE_POWER；新增 ASW 仅参加编译，未装配到控制循环。 */
app_status_t app_init(void)
{
    time_service_init();
    return (gpio_bringup_init(time_service_now_ms()) == BS_STATUS_OK)
        ? APP_STATUS_OK : APP_STATUS_ERROR;
}

app_status_t app_run_once(void)
{
    return (gpio_bringup_poll(time_service_now_ms()) == BS_STATUS_OK)
        ? APP_STATUS_OK : APP_STATUS_ERROR;
}
