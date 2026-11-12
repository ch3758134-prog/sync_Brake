#include "bsw_port.h"
#include <string.h>
/* 启动阶段单次绑定，运行期间禁止热切换；中断上下文不允许调用 Bind。 */
static BsPort active_port;
static bool port_bound;
const BsPort *Bs_PortGet(void) { return port_bound ? &active_port : NULL; }
BsStatus Bs_PortBind(const BsPort *port)
{
    if (!port) return BS_E_ARGUMENT;
    if (port_bound) return BS_E_BUSY;
    active_port = *port;
    port_bound = true;
    return BS_OK;
}
void Bs_PortUnbind(void)
{
    /* 仅供关闭系统或 Host 测试。 */
    port_bound = false;
    memset(&active_port, 0, sizeof active_port);
}
