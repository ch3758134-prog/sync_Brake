#include <assert.h>
#include <stdint.h>
#include "bsw_port.h"
#include "Wlt8016.h"
#include "BrakeSystem.h"
static int special_context = 42;
static BsStatus context_read(void *ctx, uint16_t pin, bool *level)
{
    assert(ctx == &special_context);
    assert(pin == 3u);
    *level = true;
    return BS_OK;
}
static BsStatus send_frame(void *ctx, uint8_t bus, const BsCanFrame *f)
{ (void)ctx; (void)bus; (void)f; return BS_OK; }
static BsStatus write_uart(void *ctx, uint8_t uart, const uint8_t *p, size_t n, uint32_t timeout)
{ (void)ctx; (void)uart; (void)p; (void)n; (void)timeout; return BS_OK; }
int main(void)
{
    bool level;
    BsCanFrame bad = { .id=0x800u, .dlc=8u };
    BsCanFrame good = { .id=0x123u, .dlc=8u };
    BsPort p = {0};
    Wlt8016 bt={0};
    uint8_t message[]={1u,2u};
    assert(Bs_GpioRead(0u,&level)==BS_E_UNBOUND);
    assert(Bs_CanSend(0u,&bad)==BS_E_RANGE);
    assert(Bs_CanSend(0u,&good)==BS_E_UNBOUND);
    assert(BrakeSystem_CanEnableBrakeOutput()==false);
    p.gpio_context = &special_context;
    p.gpio_read = context_read;
    p.can_send=send_frame;
    p.uart_write=write_uart;
    assert(Bs_PortBind(&p)==BS_OK);
    assert(Bs_PortBind(&p)==BS_E_BUSY);
    assert(Bs_GpioRead(3u,&level)==BS_OK && level);
    assert(Bs_CanSend(0u,&good)==BS_OK);
    assert(Wlt8016_Init(&bt,1u)==BS_OK);
    assert(Wlt8016_Send(&bt,message,sizeof message,100u)==BS_E_UNBOUND);
    Wlt8016_ReportConnection(&bt,WLT8016_CONNECTED);
    assert(Wlt8016_Send(&bt,message,sizeof message,100u)==BS_OK);
    Bs_PortUnbind();
    assert(Bs_CanSend(0u,&good)==BS_E_UNBOUND);
    return 0;
}
