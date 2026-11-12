#ifndef BRAKESYNC_BRAKESYSTEM_H
#define BRAKESYNC_BRAKESYSTEM_H
#include <stdbool.h>
typedef enum { BRAKE_SYSTEM_INHIBITED = 0 } BrakeSystemState;
/* 预研基线：尚无安全论证的制动输出必须永久禁止。 */
BrakeSystemState BrakeSystem_GetState(void);
bool BrakeSystem_CanEnableBrakeOutput(void);
#endif
