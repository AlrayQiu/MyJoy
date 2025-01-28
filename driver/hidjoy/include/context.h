
#pragma once

#include <ntddk.h>
#include <wdf.h>

#include <usb.h>
#include <wdfusb.h>

#include <hidclass.h>

#include "hid_def.h"

ELIMINATE_UNUSED_HEADER_FILE_WARNINGS();

typedef UCHAR HID_REPORT_DESCRIPTOR, *PHID_REPORT_DESCRIPTOR;

typedef struct _DEVICE_CONTEXT
{
    WDFDEVICE              Device;
    WDFQUEUE               DefaultQueue;
    WDFQUEUE               ManualQueue;
    HID_DEVICE_ATTRIBUTES  HidDeviceAttributes;
    HID_DESCRIPTOR         HidDescriptor;
    PHID_REPORT_DESCRIPTOR ReportDescriptor;
    BOOLEAN                ReadReportDescFromRegistry;
    HID_INPUT_REPORT       DeviceData;
} DEVICE_CONTEXT, *PDEVICE_CONTEXT;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(DEVICE_CONTEXT, GetDeviceContext)

typedef struct _QUEUE_CONTEXT
{
    WDFQUEUE        Queue;
    PDEVICE_CONTEXT DeviceContext;

} QUEUE_CONTEXT, *PQUEUE_CONTEXT;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(QUEUE_CONTEXT, GetQueueContext);

typedef struct _MANUAL_QUEUE_CONTEXT
{
    WDFQUEUE        Queue;
    PDEVICE_CONTEXT DeviceContext;
    WDFTIMER        Timer;

} MANUAL_QUEUE_CONTEXT, *PMANUAL_QUEUE_CONTEXT;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(MANUAL_QUEUE_CONTEXT, GetManualQueueContext);
