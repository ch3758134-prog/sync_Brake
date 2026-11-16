#include "app.h"
#include "board_support.h"

int main(void)
{
    if (board_support_init() != BOARD_STATUS_OK)
    {
        /* 初始化失败时不启动应用，等待连接调试器检查。 */
        for (;;)
        {
        }
    }

    if (app_init() != APP_STATUS_OK)
    {
        for (;;)
        {
        }
    }

    for (;;)
    {
        app_run_once();
    }
}
