#pragma once

#include <minwindef.h>
#include "WudfWdm.h"
#include "wdftypes.h"

// 只在你需要的时候使用，反正我是没用到，但是接口留着
// Use only when you need it
NTSTATUS QueueCreate(IN WDFDEVICE Device, OUT WDFQUEUE *Queue);

NTSTATUS ManualQueueCreate(IN WDFDEVICE Device, OUT WDFQUEUE *Queue);