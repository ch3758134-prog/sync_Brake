#include "BrakeSystem.h"
BrakeSystemState BrakeSystem_GetState(void) { return BRAKE_SYSTEM_INHIBITED; }
bool BrakeSystem_CanEnableBrakeOutput(void) { return false; }
