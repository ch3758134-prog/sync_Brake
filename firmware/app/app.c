#include "app.h"
#include "board_support.h"
#include "osif.h"

app_status_t app_init(void)
{
    return APP_STATUS_OK;
}

void app_run_once(void)
{
    /* 非制动 LED 心跳：PTD7 每 500 ms 翻转一次。 */
    board_support_toggle_heartbeat();
    OSIF_TimeDelay(500U);
}
