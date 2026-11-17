#include "app.h"

#include "gpio_bringup.h"
#include "time_service.h"

/* 当前 Target 是 EVB_NO_POWER 测试配置。
 * 正式制动 ASW 与执行功率驱动尚未接入调度。
 */
app_status_t app_init(void)
{
    time_service_init();
    gpio_bringup_init(time_service_now_ms());
    return APP_STATUS_OK;
}

void app_run_once(void)
{
    /* 非阻塞轮询。后续按已批准的实时预算引入正式调度。 */
    gpio_bringup_poll(time_service_now_ms());
}
