#pragma once

#include <minwindef.h>
#include "WudfWdm.h"
#include "wdftypes.h"

NTSTATUS QueueCreate(IN WDFDEVICE Device, OUT WDFQUEUE *Queue);

NTSTATUS ManualQueueCreate(IN WDFDEVICE Device, OUT WDFQUEUE *Queue);