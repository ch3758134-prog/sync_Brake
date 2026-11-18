#include "app.h"
#include "board_support.h"

int main(void)
{
    if (board_support_init() != BOARD_STATUS_OK)
    {
        /* Board 初始化失败：停止后续应用启动，不释放制动功率。 */
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
        if (app_run_once() != APP_STATUS_OK)
        {
            /* 自测错误：不继续执行其他任务，不释放制动输出。 */
            for (;;)
            {
            }
        }
    }
}
