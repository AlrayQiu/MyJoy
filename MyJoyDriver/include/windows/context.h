
#pragma once

#include <Windows.h>
#include <wdf.h>

#include <usb.h>
#include <wdfusb.h>

#include <hidport.h>

typedef UCHAR HID_REPORT_DESCRIPTOR, *PHID_REPORT_DESCRIPTOR;

typedef struct _DEVICE_CONTEXT
{
    WDFDEVICE              Device;
    WDFQUEUE               DefaultQueue;
    WDFQUEUE               ManualQueue;
    HID_DEVICE_ATTRIBUTES  HidDeviceAttributes;
    BYTE                   DeviceData;
    HID_DESCRIPTOR         HidDescriptor;
    PHID_REPORT_DESCRIPTOR ReportDescriptor;
    BOOLEAN                ReadReportDescFromRegistry;
} DEVICE_CONTEXT, *PDEVICE_CONTEXT;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(DEVICE_CONTEXT, GetDeviceContext)

typedef struct _QUEUE_CONTEXT
{
    WDFQUEUE        Queue;
    PDEVICE_CONTEXT DeviceContext;
    UCHAR           OutputReport;

} QUEUE_CONTEXT, *PQUEUE_CONTEXT;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(QUEUE_CONTEXT, GetQueueContext);

typedef struct _MANUAL_QUEUE_CONTEXT
{
    WDFQUEUE        Queue;
    PDEVICE_CONTEXT DeviceContext;
    WDFTIMER        Timer;

} MANUAL_QUEUE_CONTEXT, *PMANUAL_QUEUE_CONTEXT;

WDF_DECLARE_CONTEXT_TYPE_WITH_NAME(MANUAL_QUEUE_CONTEXT, GetManualQueueContext);
