#include <assert.h>
#include <string.h>
#include "Bma510.h"
typedef struct { unsigned count; unsigned clear; int fail; } Fake;
static int rd(void *ctx, uint8_t addr, uint8_t reg, uint8_t *out, size_t n)
{
    Fake *f=(Fake*)ctx;
    assert(addr==0x18u);
    if(f->fail) return -1;
    if(reg==0x00u && n==1u) { *out=0xc0u; f->count++; return 0; }
    if(reg==0x02u && n==1u) { *out=0x0fu; return 0; }
    if(reg==0x10u && n==1u) { *out=0u; return 0; }
    if(reg==0x11u && n==1u) { *out=1u; return 0; }
    if(reg==0x18u && n==6u) {
        const uint8_t values[6]={0u,0x40u,0u,0xc0u,0u,0u};
        memcpy(out,values,6u); return 0;
    }
    return -1;
}
static int wr(void *ctx, uint8_t addr, uint8_t reg, const uint8_t *data, size_t n)
{
    Fake *f=(Fake*)ctx; assert(addr==0x18u);
    if(f->fail || n!=1u) return -1;
    if(reg==0x11u && *data==1u) f->clear++;
    return 0;
}
static void delay(void *ctx,uint32_t ms) { (void)ctx; assert(ms>=3u); }
int main(void)
{
    Fake f={0};
    Bma510 d={0};
    Bma510Sample sample={0};
    d.bus.context=&f; d.bus.read=rd; d.bus.write=wr; d.bus.delay_ms=delay;
    assert(Bma510_Init(&d)==BMA510_OK && f.count>=2u);
    assert(Bma510_Read(&d,&sample)==BMA510_OK);
    assert(sample.x==16384 && sample.y==-16384 && sample.z==0 && f.clear==1u);
    assert(Bma510_ToMilliG(sample.x)==1000);
    f.fail=1;
    assert(Bma510_Read(&d,&sample)==BMA510_E_IO);
    return 0;
}
