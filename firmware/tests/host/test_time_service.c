#include "time_service.h"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>

static uint32_t fake_now_ms;
static uint32_t initialization_count;

/* Host 假 MCAL 与正式 YTM32 SDK 适配永不一起参与 Target 构建。 */
void mcal_time_init(void)
{
    initialization_count++;
}

uint32_t mcal_time_now_ms(void)
{
    return fake_now_ms;
}

int main(void)
{
    time_service_init();
    assert(initialization_count == 1U);

    fake_now_ms = 1234U;
    assert(time_service_now_ms() == 1234U);

    assert(time_service_has_elapsed(100U, 10U, 110U));
    assert(!time_service_has_elapsed(100U, 10U, 109U));
    assert(!time_service_has_elapsed(110U, 10U, 100U));
    assert(time_service_has_elapsed(UINT32_MAX - 4U, 10U, 5U));
    assert(!time_service_has_elapsed(0U, UINT32_C(0x80000000), 1U));
    assert(!time_service_has_elapsed(0U, 5U, UINT32_C(0x80000000)));

    puts("Time service Host behavior tests: PASS");
    return 0;
}
