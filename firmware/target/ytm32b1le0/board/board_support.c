#include "board_support.h"
#include "sdk_project_config.h"
#include "interrupt_manager.h"

#define HEARTBEAT_LED_PORT      (GPIOD)
#define HEARTBEAT_LED_PIN       (7U)
#define BUTTON_LED_PORT         (GPIOD)
#define BUTTON_LED_PIN          (6U)
#define BUTTON_PORT             (GPIOE)
#define BUTTON_SW1_PIN          (0U)
#define BUTTON_SW2_PIN          (1U)

static volatile uint32_t button_irq_count;

board_status_t board_support_init(void)
{
    CLOCK_SYS_Init(g_clockManConfigsArr, CLOCK_MANAGER_CONFIG_CNT,
                   g_clockManCallbacksArr, CLOCK_MANAGER_CALLBACK_CNT);

    if (CLOCK_SYS_UpdateConfiguration(CLOCK_MANAGER_ACTIVE_INDEX,
                                      CLOCK_MANAGER_POLICY_AGREEMENT) != STATUS_SUCCESS)
    {
        return BOARD_STATUS_CLOCK_ERROR;
    }

    if (PINS_DRV_Init(NUM_OF_CONFIGURED_PINS0,
                      g_pin_mux_InitConfigArr0) != STATUS_SUCCESS)
    {
        return BOARD_STATUS_PINS_ERROR;
    }

    button_irq_count = 0U;
    PINS_DRV_ClearPinIntFlagCmd(BUTTON_PORT, BUTTON_SW1_PIN);
    PINS_DRV_ClearPinIntFlagCmd(BUTTON_PORT, BUTTON_SW2_PIN);
    INT_SYS_EnableIRQ(GPIO_IRQn);

    return BOARD_STATUS_OK;
}

void board_support_toggle_heartbeat(void)
{
    PINS_DRV_TogglePins(HEARTBEAT_LED_PORT, (1UL << HEARTBEAT_LED_PIN));
}

void GPIO_IRQHandler(void)
{
    uint32_t pending = PINS_DRV_GetPortIntFlag(BUTTON_PORT);
    uint32_t button_mask = (1UL << BUTTON_SW1_PIN) | (1UL << BUTTON_SW2_PIN);

    if ((pending & (1UL << BUTTON_SW1_PIN)) != 0U)
    {
        PINS_DRV_ClearPinIntFlagCmd(BUTTON_PORT, BUTTON_SW1_PIN);
    }
    if ((pending & (1UL << BUTTON_SW2_PIN)) != 0U)
    {
        PINS_DRV_ClearPinIntFlagCmd(BUTTON_PORT, BUTTON_SW2_PIN);
    }

    if ((pending & button_mask) != 0U)
    {
        /* SW1/SW2 共用一个观察 LED；实际产品按键业务另行设计。 */
        PINS_DRV_TogglePins(BUTTON_LED_PORT, (1UL << BUTTON_LED_PIN));
        if (button_irq_count != UINT32_MAX)
        {
            button_irq_count++;
        }
    }
}

uint32_t board_support_get_button_irq_count(void)
{
    /* Cortex-M0+ 上自然对齐的 32 位计数读取是单次访问。 */
    return button_irq_count;
}
