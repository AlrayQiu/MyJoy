#include "hid_def/default/myjoy_default_hid_def.h"

#include "device.h"
#include "WudfWdm.h"
#include "queue.h"

#include "common.h"
#include "context.h"
#include "utils.h"

#include "wdffdo.h"

NTSTATUS MyJoyDeviceAdd(IN WDFDRIVER Driver, IN OUT PWDFDEVICE_INIT DeviceInit)
{
    NTSTATUS               status;
    WDF_OBJECT_ATTRIBUTES  deviceAttributes;
    WDFDEVICE              device;
    PDEVICE_CONTEXT        deviceContext;
    PHID_DEVICE_ATTRIBUTES hidAttributes;
    UNREFERENCED_PARAMETER(Driver);

    KdPrint(("Enter MyJoyDeviceAdd\n"));

    WdfFdoInitSetFilter(DeviceInit);

    WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&deviceAttributes, DEVICE_CONTEXT);

    status = WdfDeviceCreate(&DeviceInit, &deviceAttributes, &device);

    ASSERT_STATUS_SUCCESS(status, "WdfDeviceCreate failed");

    deviceContext             = GetDeviceContext(device);
    deviceContext->Device     = device;
    deviceContext->DeviceData = 0;

    hidAttributes = &deviceContext->HidDeviceAttributes;
    RtlZeroMemory(hidAttributes, sizeof(HID_DEVICE_ATTRIBUTES));
    hidAttributes->Size          = sizeof(HID_DEVICE_ATTRIBUTES);
    hidAttributes->VendorID      = MYJOY_VID;
    hidAttributes->ProductID     = MYJOY_PID;
    hidAttributes->VersionNumber = MYJOY_VERSION;

    status = QueueCreate(device, &deviceContext->DefaultQueue);
    if (!NT_SUCCESS(status))
        return status;

    deviceContext->HidDescriptor = G_MyJoyHidDescriptor;

    return status;
}