#pragma once

#include <ntddk.h>
#include <wdf.h>

NTSTATUS QueueCreate(IN WDFDEVICE Device, OUT WDFQUEUE *Queue);

NTSTATUS ManualQueueCreate(IN WDFDEVICE Device, OUT WDFQUEUE *Queue);