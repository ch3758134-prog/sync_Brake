#include "gpio_bringup.h"

#include "interrupt_manager.h"
#include "pins_driver.h"
#include "time_service.h"

#include <stdint.h>

#define HEARTBEAT_LED_PORT   (GPIOD)
#define HEARTBEAT_LED_PIN    (7U)
#define BUTTON_LED_PORT      (GPIOD)
#define BUTTON_LED_PIN       (6U)
#define BUTTON_PORT          (GPIOE)
#define BUTTON_SW1_PIN       (0U)
#define BUTTON_SW2_PIN       (1U)
#define HEARTBEAT_PERIOD_MS  (500U)

static volatile uint32_t button_irq_count;
static uint32_t last_heartbeat_ms;

void gpio_bringup_init(uint32_t now_ms)
{
    button_irq_count = 0U;
    last_heartbeat_ms = now_ms;

    PINS_DRV_ClearPinIntFlagCmd(BUTTON_PORT, BUTTON_SW1_PIN);
    PINS_DRV_ClearPinIntFlagCmd(BUTTON_PORT, BUTTON_SW2_PIN);
    INT_SYS_EnableIRQ(GPIO_IRQn);
}

void gpio_bringup_poll(uint32_t now_ms)
{
    if (time_service_has_elapsed(last_heartbeat_ms, HEARTBEAT_PERIOD_MS, now_ms))
    {
        last_heartbeat_ms = now_ms;
        PINS_DRV_TogglePins(HEARTBEAT_LED_PORT, (1UL << HEARTBEAT_LED_PIN));
    }
}

void GPIO_IRQHandler(void)
{
    const uint32_t pending = PINS_DRV_GetPortIntFlag(BUTTON_PORT);
    const uint32_t mask = (1UL << BUTTON_SW1_PIN) | (1UL << BUTTON_SW2_PIN);

    if ((pending & (1UL << BUTTON_SW1_PIN)) != 0U)
    {
        PINS_DRV_ClearPinIntFlagCmd(BUTTON_PORT, BUTTON_SW1_PIN);
    }

    if ((pending & (1UL << BUTTON_SW2_PIN)) != 0U)
    {
        PINS_DRV_ClearPinIntFlagCmd(BUTTON_PORT, BUTTON_SW2_PIN);
    }

    if ((pending & mask) != 0U)
    {
        /* EVB 观测用途：板级按键触发反馈灯，不涉及功率输出。 */
        PINS_DRV_TogglePins(BUTTON_LED_PORT, (1UL << BUTTON_LED_PIN));
        if (button_irq_count != UINT32_MAX)
        {
            button_irq_count++;
        }
    }
}

uint32_t gpio_bringup_get_button_irq_count(void)
{
    /* 目标 Cortex-M0+ 自然对齐的 32-bit 单次读取，仍需 Target 验证。 */
    return button_irq_count;
}
