#include "system_init.h"
#include "sdk_project_config.h"

/*
 * 本板的 GPIO 由 EAL_GPIO_Init() 经 MCAL 统一配置。
 * YCT 的非 GPIO 外设复用仍由 system_init() 应用。
 * 不再在这里初始化 YCT GPIO 行，避免一根引脚重复初始化。
 */
system_init_status_t system_init(void)
{
    uint32_t i;

    CLOCK_SYS_Init(g_clockManConfigsArr,
                   CLOCK_MANAGER_CONFIG_CNT,
                   g_clockManCallbacksArr,
                   CLOCK_MANAGER_CALLBACK_CNT);

    if (CLOCK_SYS_UpdateConfiguration(CLOCK_MANAGER_ACTIVE_INDEX,
                                      CLOCK_MANAGER_POLICY_AGREEMENT) != STATUS_SUCCESS) {
        return SYSTEM_INIT_CLOCK_ERROR;
    }

    for (i = 0U; i < NUM_OF_CONFIGURED_PINS0; ++i) {
        const pin_settings_config_t *config = &g_pin_mux_InitConfigArr0[i];
        if (config->mux != PCTRL_MUX_AS_GPIO) {
            if (PINS_DRV_Init(1U, config) != STATUS_SUCCESS) {
                return SYSTEM_INIT_PINS_ERROR;
            }
        }
    }

    return SYSTEM_INIT_OK;
}