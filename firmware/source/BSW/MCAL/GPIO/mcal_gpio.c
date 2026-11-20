#include "mcal_gpio.h"

#include "device_registers.h"
#include "interrupt_manager.h"
#include "pins_driver.h"

#define GPIO_IRQ_SLOT_COUNT (8U)

typedef struct
{
    mcal_gpio_port_t port;
    uint8_t pin;
    bool used;
    volatile uint32_t count;
    volatile bool overflowed;
} gpio_irq_slot_t;

static GPIO_Type *const s_gpio_base[MCAL_GPIO_PORT_COUNT] = {
    GPIOA, GPIOB, GPIOC, GPIOD, GPIOE
};

static PCTRL_Type *const s_pctrl_base[MCAL_GPIO_PORT_COUNT] = {
    PCTRLA, PCTRLB, PCTRLC, PCTRLD, PCTRLE
};

static gpio_irq_slot_t s_irq_slots[GPIO_IRQ_SLOT_COUNT];

static bool IsValidPin(mcal_gpio_port_t port, uint8_t pin)
{
    return ((uint32_t)port < (uint32_t)MCAL_GPIO_PORT_COUNT) &&
           ((uint32_t)pin < (uint32_t)PCTRL_PCR_COUNT);
}

static bool IsValidConfig(const mcal_gpio_config_t *config)
{
    return (config != (const mcal_gpio_config_t *)0) &&
           ((uint32_t)config->direction <= (uint32_t)MCAL_GPIO_OUTPUT) &&
           ((uint32_t)config->pull <= (uint32_t)MCAL_GPIO_PULL_UP) &&
           ((uint32_t)config->drive <= (uint32_t)MCAL_GPIO_DRIVE_HIGH) &&
           ((uint32_t)config->slew <= (uint32_t)MCAL_GPIO_SLEW_SLOW);
}

bool MCAL_GPIO_Init(mcal_gpio_port_t port, uint8_t pin,
                    const mcal_gpio_config_t *config)
{
    pin_settings_config_t sdk = {0};

    if (!IsValidPin(port, pin) || !IsValidConfig(config)) {
        return false;
    }

    sdk.base = s_pctrl_base[port];
    sdk.gpioBase = s_gpio_base[port];
    sdk.pinPortIdx = pin;
    sdk.mux = PCTRL_MUX_AS_GPIO;
    sdk.direction = (config->direction == MCAL_GPIO_OUTPUT)
                        ? GPIO_OUTPUT_DIRECTION : GPIO_INPUT_DIRECTION;
    sdk.initValue = (config->initial_level != 0U) ? 1U : 0U;

#if FEATURE_PINS_HAS_PULL_SELECTION
    switch (config->pull) {
        case MCAL_GPIO_PULL_DOWN:
            sdk.pullConfig = PCTRL_INTERNAL_PULL_DOWN_ENABLED;
            break;
        case MCAL_GPIO_PULL_UP:
            sdk.pullConfig = PCTRL_INTERNAL_PULL_UP_ENABLED;
            break;
        default:
            sdk.pullConfig = PCTRL_INTERNAL_PULL_NOT_ENABLED;
            break;
    }
#else
    if (config->pull != MCAL_GPIO_PULL_NONE) {
        return false;
    }
#endif

#if FEATURE_PINS_HAS_DRIVE_STRENGTH
    sdk.driveSelect = (config->drive == MCAL_GPIO_DRIVE_HIGH)
                          ? PCTRL_HIGH_DRIVE_STRENGTH : PCTRL_LOW_DRIVE_STRENGTH;
#else
    if (config->drive != MCAL_GPIO_DRIVE_LOW) {
        return false;
    }
#endif

#if FEATURE_PINS_HAS_SLEW_RATE
    sdk.rateSelect = (config->slew == MCAL_GPIO_SLEW_SLOW)
                         ? PCTRL_SLOW_SLEW_RATE : PCTRL_FAST_SLEW_RATE;
#else
    if (config->slew != MCAL_GPIO_SLEW_FAST) {
        return false;
    }
#endif

#if FEATURE_PINS_HAS_PASSIVE_FILTER
    sdk.passiveFilter = config->passive_filter;
#else
    if (config->passive_filter) {
        return false;
    }
#endif

#if (FEATURE_PINS_HAS_DIGITAL_FILTER) || (FEATURE_PCTRL_HAS_DIGITAL_FILTER)
    sdk.digitalFilter = config->digital_filter;
    sdk.filterConfig.width = config->digital_filter_width;
#else
    if (config->digital_filter) {
        return false;
    }
    sdk.digitalFilter = false;
#endif

    /* IRQ 必须在登记事件槽之后才能启用：防止 ISR 漏处理。 */
    sdk.intConfig = PCTRL_DMA_INT_DISABLED;
    sdk.clearIntFlag = true;

    /* 参数确认后才操作硬件。先预装输出锁存值，降低初始化毛刺风险。 */
    if (config->direction == MCAL_GPIO_OUTPUT) {
        PINS_DRV_WritePin(s_gpio_base[port], pin,
                          (config->initial_level != 0U) ? 1U : 0U);
    }

    return PINS_DRV_Init(1U, &sdk) == STATUS_SUCCESS;
}

void MCAL_GPIO_Write(mcal_gpio_port_t port, uint8_t pin, uint8_t level)
{
    if (!IsValidPin(port, pin)) {
        return;
    }
    PINS_DRV_WritePin(s_gpio_base[port], pin, (level != 0U) ? 1U : 0U);
}

uint8_t MCAL_GPIO_Read(mcal_gpio_port_t port, uint8_t pin)
{
    if (!IsValidPin(port, pin)) {
        return 0U;
    }
    return (PINS_DRV_ReadPin(s_gpio_base[port], pin) != 0U) ? 1U : 0U;
}

void MCAL_GPIO_Toggle(mcal_gpio_port_t port, uint8_t pin)
{
    if (!IsValidPin(port, pin)) {
        return;
    }
    PINS_DRV_TogglePins(s_gpio_base[port], 1UL << pin);
}

static gpio_interrupt_config_t ToSdkInterruptMode(mcal_gpio_irq_mode_t mode)
{
    switch (mode) {
        case MCAL_GPIO_IRQ_RISING:     return PCTRL_INT_RISING_EDGE;
        case MCAL_GPIO_IRQ_FALLING:    return PCTRL_INT_FALLING_EDGE;
        case MCAL_GPIO_IRQ_BOTH_EDGES: return PCTRL_INT_EITHER_EDGE;
        default:                      return PCTRL_DMA_INT_DISABLED;
    }
}

static gpio_irq_slot_t *FindSlot(mcal_gpio_port_t port, uint8_t pin)
{
    uint32_t i;
    for (i = 0U; i < GPIO_IRQ_SLOT_COUNT; ++i) {
        if (s_irq_slots[i].used &&
            s_irq_slots[i].port == port && s_irq_slots[i].pin == pin) {
            return &s_irq_slots[i];
        }
    }
    return (gpio_irq_slot_t *)0;
}

bool MCAL_GPIO_ConfigureInterrupt(mcal_gpio_port_t port, uint8_t pin,
                                  mcal_gpio_irq_mode_t mode)
{
    uint32_t i;
    uint32_t previous_mask;
    gpio_irq_slot_t *slot;

    if (!IsValidPin(port, pin) || (uint32_t)mode > (uint32_t)MCAL_GPIO_IRQ_BOTH_EDGES) {
        return false;
    }

    /* 保存原 PRIMASK：不能擅自打开调用者已关闭的全局中断。 */
    previous_mask = __get_PRIMASK();
    __disable_irq();

    slot = FindSlot(port, pin);
    if (mode != MCAL_GPIO_IRQ_DISABLED && slot == (gpio_irq_slot_t *)0) {
        for (i = 0U; i < GPIO_IRQ_SLOT_COUNT; ++i) {
            if (!s_irq_slots[i].used) {
                slot = &s_irq_slots[i];
                break;
            }
        }
        if (slot == (gpio_irq_slot_t *)0) {
            __set_PRIMASK(previous_mask);
            return false;
        }
    }

    PINS_DRV_SetPinIntSel(s_gpio_base[port], pin, PCTRL_DMA_INT_DISABLED);
    PINS_DRV_ClearPinIntFlagCmd(s_gpio_base[port], pin);

    if (slot != (gpio_irq_slot_t *)0) {
        slot->count = 0U;
        slot->overflowed = false;
        slot->port = port;
        slot->pin = pin;
        slot->used = (mode != MCAL_GPIO_IRQ_DISABLED);
    }

    if (mode != MCAL_GPIO_IRQ_DISABLED) {
        PINS_DRV_SetPinIntSel(s_gpio_base[port], pin, ToSdkInterruptMode(mode));
    }

    __set_PRIMASK(previous_mask);
    return true;
}

void MCAL_GPIO_EnableInterruptController(void)
{
    /* 在全部 GPIO IRQ 引脚配置完成后调用一次。 */
    INT_SYS_ClearPending(GPIO_IRQn);
    INT_SYS_EnableIRQ(GPIO_IRQn);
}

mcal_gpio_events_t MCAL_GPIO_TakeEvents(mcal_gpio_port_t port, uint8_t pin)
{
    mcal_gpio_events_t events = {0U, false};
    uint32_t previous_mask;
    gpio_irq_slot_t *slot;

    if (!IsValidPin(port, pin)) {
        return events;
    }

    previous_mask = __get_PRIMASK();
    __disable_irq();
    slot = FindSlot(port, pin);
    if (slot != (gpio_irq_slot_t *)0) {
        events.count = slot->count;
        events.overflowed = slot->overflowed;
        slot->count = 0U;
        slot->overflowed = false;
    }
    __set_PRIMASK(previous_mask);
    return events;
}

/* 唯一 GPIO_IRQn ISR；不能同时编译旧 legacy/evb_gpio_driver.c。 */
void GPIO_IRQHandler(void)
{
    uint32_t port_index;
    uint32_t i;

    for (port_index = 0U; port_index < (uint32_t)MCAL_GPIO_PORT_COUNT; ++port_index) {
        uint32_t flags = PINS_DRV_GetPortIntFlag(s_gpio_base[port_index]);
        if (flags == 0U) {
            continue;
        }

        /* 本 MCAL 独占当前 GPIO IRQ；先清标志再累计事件。 */
        PINS_DRV_ClearPortIntFlagCmd(s_gpio_base[port_index]);
        for (i = 0U; i < GPIO_IRQ_SLOT_COUNT; ++i) {
            gpio_irq_slot_t *slot = &s_irq_slots[i];
            if (!slot->used || (uint32_t)slot->port != port_index ||
                (flags & (1UL << slot->pin)) == 0U) {
                continue;
            }
            if (slot->count == UINT32_MAX) {
                slot->overflowed = true;
            } else {
                ++slot->count;
            }
        }
    }
}