#include "test_gpio.h"
#include "eal_gpio.h"
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
static uint32_t init_count;
static uint32_t pending1, pending2;
static uint8_t led0, led1;

/* Host EAL mock：不连接 MCU，也不装入真正 GPIO ISR。 */
bool EAL_GPIO_Init(void) { ++init_count; led0 = 0U; led1 = 0U; return true; }
void EAL_GPIO_WriteLed(eal_gpio_led_t id, uint8_t v) {
    if (id == EAL_GPIO_LED_HEARTBEAT) led0 = v;
    if (id == EAL_GPIO_LED_BUTTON) led1 = v;
}
void EAL_GPIO_ToggleLed(eal_gpio_led_t id) {
    if (id == EAL_GPIO_LED_HEARTBEAT) led0 ^= 1U;
    if (id == EAL_GPIO_LED_BUTTON) led1 ^= 1U;
}
uint8_t EAL_GPIO_ReadButton(eal_gpio_button_t id) {
    return (id == EAL_GPIO_BUTTON_1) ? 1U : 0U;
}
eal_gpio_events_t EAL_GPIO_TakeButtonEvents(eal_gpio_button_t id) {
    eal_gpio_events_t ev={0U,false};
    if(id==EAL_GPIO_BUTTON_1){ev.count=pending1;pending1=0U;}
    else if(id==EAL_GPIO_BUTTON_2){ev.count=pending2;pending2=0U;}
    return ev;
}
bool time_service_has_elapsed(uint32_t start, uint32_t period, uint32_t now){
    uint32_t delta=now-start;
    return (period<UINT32_C(0x80000000) && delta<UINT32_C(0x80000000) && delta>=period);
}
int main(void){
    test_gpio_stats_t state={0};
    assert(test_gpio_get_stats(&state)==BS_STATUS_NOT_INITIALIZED);
    assert(test_gpio_init(100U)==BS_STATUS_OK);
    assert(init_count==1U);
    assert(test_gpio_run_once(100U)==BS_STATUS_OK);
    assert(!led0 && !led1);
    pending1=2U;
    assert(test_gpio_run_once(600U)==BS_STATUS_OK);
    assert(led0 && led1);
    assert(test_gpio_get_stats(&state)==BS_STATUS_OK);
    assert(state.processed_button_events==2U && state.poll_count==2U);
    assert(state.button_1_level && !state.button_2_level);
    assert(test_gpio_get_stats(NULL)==BS_STATUS_INVALID_ARGUMENT);
    return 0;
}
