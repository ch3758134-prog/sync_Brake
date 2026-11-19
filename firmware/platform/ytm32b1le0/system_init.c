#include "system_init.h"

#include "sdk_project_config.h"

/* 板级配置由上传的官方 YCT 示例生成；本文件不复制 SDK 实现。 */
system_init_status_t system_init(void)
{
    CLOCK_SYS_Init(g_clockManConfigsArr,
                   CLOCK_MANAGER_CONFIG_CNT,
                   g_clockManCallbacksArr,
                   CLOCK_MANAGER_CALLBACK_CNT);

    if (CLOCK_SYS_UpdateConfiguration(CLOCK_MANAGER_ACTIVE_INDEX,
                                      CLOCK_MANAGER_POLICY_AGREEMENT) != STATUS_SUCCESS)
    {
        return SYSTEM_INIT_CLOCK_ERROR;
    }

    if (PINS_DRV_Init(NUM_OF_CONFIGURED_PINS0,
                      g_pin_mux_InitConfigArr0) != STATUS_SUCCESS)
    {
        return SYSTEM_INIT_PINS_ERROR;
    }

    return SYSTEM_INIT_OK;
}
