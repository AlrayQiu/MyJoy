#include "default/myJoy_default_hid_report_descriptor.h"
#include "hid_def/hid_def.h"

#include "device.h"
#include "queue.h"

#include "context.h"
#include "utils.h"

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

    deviceContext                   = GetDeviceContext(device);
    deviceContext->Device           = device;
    deviceContext->HidDescriptor    = S_MyJoyHidDescriptor;
    deviceContext->ReportDescriptor = S_ReportDescriptor;

    hidAttributes = &deviceContext->HidDeviceAttributes;
    RtlZeroMemory(hidAttributes, sizeof(HID_DEVICE_ATTRIBUTES));
    hidAttributes->Size          = sizeof(HID_DEVICE_ATTRIBUTES);
    hidAttributes->VendorID      = MYJOY_VID;
    hidAttributes->ProductID     = MYJOY_PID;
    hidAttributes->VersionNumber = MYJOY_VERSION;

    status = QueueCreate(device, &deviceContext->DefaultQueue);
    if (!NT_SUCCESS(status))
        return status;

    status = ManualQueueCreate(device, &deviceContext->ManualQueue);
    if (!NT_SUCCESS(status))
        return status;

    KdPrint(("End MyJoyDeviceAdd:0x%x\r\n", status));

    return status;
}