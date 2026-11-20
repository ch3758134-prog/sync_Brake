#include "eal_gpio.h"
#include "mcal_gpio.h"

typedef struct {
    mcal_gpio_port_t port;
    uint8_t pin;
} gpio_binding_t;

/* 来源：YTM32B1LE05 EVB 的 YCT 引脚表；只是开发板映射。 */
static const gpio_binding_t s_leds[EAL_GPIO_LED_COUNT] = {
    {MCAL_GPIO_PORT_D, 7U},  /* PTD7: 心跳 LED */
    {MCAL_GPIO_PORT_D, 6U}   /* PTD6: 按键事件 LED */
};

static const gpio_binding_t s_buttons[EAL_GPIO_BUTTON_COUNT] = {
    {MCAL_GPIO_PORT_E, 0U},  /* PTE0: SW1 */
    {MCAL_GPIO_PORT_E, 1U}   /* PTE1: SW2 */
};

/* LED 使用当前 YCT 已给出的电气值，初始低电平是 MCU 原始电平。 */
static const mcal_gpio_config_t s_led_config = {
    .direction = MCAL_GPIO_OUTPUT,
    .initial_level = 0U,
    .pull = MCAL_GPIO_PULL_NONE,
    .drive = MCAL_GPIO_DRIVE_LOW,
    .slew = MCAL_GPIO_SLEW_FAST,
    .passive_filter = false,
    .digital_filter = false,
    .digital_filter_width = 0U
};

/* 复现当前 YCT 按键配置；宽度 0 不代表已实现机械消抖。 */
static const mcal_gpio_config_t s_button_config = {
    .direction = MCAL_GPIO_INPUT,
    .initial_level = 0U,
    .pull = MCAL_GPIO_PULL_NONE,
    .drive = MCAL_GPIO_DRIVE_LOW,
    .slew = MCAL_GPIO_SLEW_FAST,
    .passive_filter = false,
    .digital_filter = true,
    .digital_filter_width = 0U
};

bool EAL_GPIO_Init(void)
{
    uint32_t i;

    /* 先初始化全部已确认的 GPIO 输出，并明确给出上电软件初始状态。 */
    for (i = 0U; i < (uint32_t)EAL_GPIO_LED_COUNT; ++i) {
        if (!MCAL_GPIO_Init(s_leds[i].port, s_leds[i].pin, &s_led_config)) {
            return false;
        }
    }

    /* PTE6 是原 GPIO Demo 自测脚；当前无业务使用，保持低电平。 */
    if (!MCAL_GPIO_Init(MCAL_GPIO_PORT_E, 6U, &s_led_config)) {
        return false;
    }

    /* 再初始化按钮输入的电气配置。 */
    for (i = 0U; i < (uint32_t)EAL_GPIO_BUTTON_COUNT; ++i) {
        if (!MCAL_GPIO_Init(s_buttons[i].port, s_buttons[i].pin,
                            &s_button_config)) {
            return false;
        }
    }

    /* 最后使能按键中断：注册时清标志，NVIC 总开关最终打开。 */
    if (!MCAL_GPIO_ConfigureInterrupt(
            s_buttons[0].port, s_buttons[0].pin, MCAL_GPIO_IRQ_RISING)) {
        return false;
    }
    if (!MCAL_GPIO_ConfigureInterrupt(
            s_buttons[1].port, s_buttons[1].pin, MCAL_GPIO_IRQ_RISING)) {
        (void)MCAL_GPIO_ConfigureInterrupt(
            s_buttons[0].port, s_buttons[0].pin, MCAL_GPIO_IRQ_DISABLED);
        return false;
    }

    MCAL_GPIO_EnableInterruptController();
    return true;
}

void EAL_GPIO_WriteLed(eal_gpio_led_t led, uint8_t raw_level)
{
    if ((uint32_t)led >= (uint32_t)EAL_GPIO_LED_COUNT) {
        return;
    }
    MCAL_GPIO_Write(s_leds[led].port, s_leds[led].pin, raw_level);
}

void EAL_GPIO_ToggleLed(eal_gpio_led_t led)
{
    if ((uint32_t)led >= (uint32_t)EAL_GPIO_LED_COUNT) {
        return;
    }
    MCAL_GPIO_Toggle(s_leds[led].port, s_leds[led].pin);
}

uint8_t EAL_GPIO_ReadButton(eal_gpio_button_t button)
{
    if ((uint32_t)button >= (uint32_t)EAL_GPIO_BUTTON_COUNT) {
        return 0U;
    }
    return MCAL_GPIO_Read(s_buttons[button].port, s_buttons[button].pin);
}

eal_gpio_events_t EAL_GPIO_TakeButtonEvents(eal_gpio_button_t button)
{
    eal_gpio_events_t result = {0U, false};
    mcal_gpio_events_t events;

    if ((uint32_t)button >= (uint32_t)EAL_GPIO_BUTTON_COUNT) {
        return result;
    }
    events = MCAL_GPIO_TakeEvents(s_buttons[button].port, s_buttons[button].pin);
    result.count = events.count;
    result.overflowed = events.overflowed;
    return result;
}