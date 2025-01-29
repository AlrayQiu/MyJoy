#include "bus_driver.h"
#include "bus_public.h"
#include "child.h"
#include "wmi.h"

EVT_WDF_IO_QUEUE_IO_INTERNAL_DEVICE_CONTROL EvtIoDeviceControl;

#ifdef ALLOC_PRAGMA
#pragma alloc_text(PAGE, BusDeviceAdd)
#pragma alloc_text(PAGE, EvtIoDeviceControl)
#endif
NTSTATUS
BusDeviceAdd(WDFDRIVER Driver, PWDFDEVICE_INIT DeviceInit)
{
    NTSTATUS              status = STATUS_SUCCESS;
    WDF_CHILD_LIST_CONFIG childConfig;
    WDF_OBJECT_ATTRIBUTES fdoAttributes;
    WDFDEVICE             device;
    WDF_IO_QUEUE_CONFIG   queueConfig;
    PNP_BUS_INFORMATION   busInfomation;
    WDFQUEUE              queue;

    UNREFERENCED_PARAMETER(Driver);

    PAGED_CODE();

    KdPrint(("BusJoy Device Add\r\n"));

    WdfDeviceInitSetDeviceType(DeviceInit, FILE_DEVICE_BUS_EXTENDER);
    WdfDeviceInitSetExclusive(DeviceInit, TRUE);

    WDF_CHILD_LIST_CONFIG_INIT(&childConfig, sizeof(CHILD_PDO_IDENTIFICATION_DESCRIPTION), BusEvtDeviceListCreatePdo);
    childConfig.EvtChildListIdentificationDescriptionDuplicate = BusEvtChildListIdentificationDescriptionDuplicate;
    childConfig.EvtChildListIdentificationDescriptionCompare   = BusEvtChildListIdentificationDescriptionCompare;
    childConfig.EvtChildListIdentificationDescriptionCleanup   = BusEvtChildListIdentificationDescriptionCleanup;
    WdfFdoInitSetDefaultChildListConfig(DeviceInit, &childConfig, WDF_NO_OBJECT_ATTRIBUTES);

    WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&fdoAttributes, BUS_FDO_DEVICE_DATA);

    status = WdfDeviceCreate(&DeviceInit, &fdoAttributes, &device);

    if (!NT_SUCCESS(status))
    {
        KdPrint(("Error creating device 0x%x\n", status));
        return status;
    }

    WDF_IO_QUEUE_CONFIG_INIT_DEFAULT_QUEUE(&queueConfig, WdfIoQueueDispatchParallel);

    queueConfig.EvtIoDeviceControl = EvtIoDeviceControl;

    __analysis_assume(queueConfig.EvtIoStop != 0);
    status = WdfIoQueueCreate(device, &queueConfig, WDF_NO_OBJECT_ATTRIBUTES, &queue);
    __analysis_assume(queueConfig.EvtIoStop == 0);

    if (!NT_SUCCESS(status))
    {
        KdPrint(("WdfIoQueueCreate failed status 0x%x\n", status));
        return status;
    }

    status = WdfDeviceCreateDeviceInterface(device, &GUID_DEVINTERFACE_BUSJOY, NULL);
    if (!NT_SUCCESS(status))
    {
        return status;
    }

    busInfomation.BusTypeGuid   = GUID_DEVCLASS_BUSJOY;
    busInfomation.LegacyBusType = PNPBus;
    busInfomation.BusNumber     = 0;

    WdfDeviceSetBusInformationForChildren(device, &busInfomation);
    status = BusWmiRegistration(device);
    if (!NT_SUCCESS(status))
    {
        return status;
    }

    return status;
}

VOID EvtIoDeviceControl(IN WDFQUEUE   Queue,
                        IN WDFREQUEST Request,
                        IN size_t     OutputBufferLength,
                        IN size_t     InputBufferLength,
                        IN ULONG      IoControlCode)
{
    NTSTATUS                 status = STATUS_INVALID_PARAMETER;
    WDFDEVICE                hDevice;
    size_t                   length = 0;
    PBUSENUM_PLUGIN_HARDWARE plugIn = NULL;
    PBUSENUM_UNPLUG_HARDWARE unPlug = NULL;
    PBUSENUM_EJECT_HARDWARE  eject  = NULL;

    UNREFERENCED_PARAMETER(OutputBufferLength);

    PAGED_CODE();

    hDevice = WdfIoQueueGetDevice(Queue);

    KdPrint(("Bus_EvtIoDeviceControl: 0x%p\n", hDevice));

    switch (IoControlCode)
    {
    case IOCTL_BUSENUM_PLUGIN_HARDWARE:
        status = WdfRequestRetrieveInputBuffer(
            Request,
            sizeof(BUSENUM_PLUGIN_HARDWARE) + (sizeof(UNICODE_NULL) * 2), // 2 for double NULL termination (MULTI_SZ)
            (VOID *)&plugIn,
            &length);
        if (!NT_SUCCESS(status))
        {
            KdPrint(("WdfRequestRetrieveInputBuffer failed 0x%x\n", status));
            break;
        }

        ASSERT(length == InputBufferLength);

        if (sizeof(BUSENUM_PLUGIN_HARDWARE) == plugIn->Size)
        {
            length = (InputBufferLength - sizeof(BUSENUM_PLUGIN_HARDWARE)) / sizeof(WCHAR);
            //
            // Make sure the IDs is two NULL terminated.
            //
            if ((UNICODE_NULL != plugIn->HardwareIDs[length - 1]) || (UNICODE_NULL != plugIn->HardwareIDs[length - 2]))
            {
                status = STATUS_INVALID_PARAMETER;
                break;
            }

            status = BusPlugInDevice(hDevice, plugIn->HardwareIDs, length, plugIn->SerialNo);
        }
        break;
    case IOCTL_BUSENUM_UNPLUG_HARDWARE:
        status = WdfRequestRetrieveInputBuffer(Request, sizeof(BUSENUM_UNPLUG_HARDWARE), (VOID *)&unPlug, &length);
        if (!NT_SUCCESS(status))
        {
            KdPrint(("WdfRequestRetrieveInputBuffer failed 0x%x\n", status));
            break;
        }

        if (unPlug->Size == InputBufferLength)
        {
            status = BusUnPlugDevice(hDevice, unPlug->SerialNo);
        }

        break;
    case IOCTL_BUSENUM_EJECT_HARDWARE:
        status = WdfRequestRetrieveInputBuffer(Request, sizeof(BUSENUM_EJECT_HARDWARE), (VOID *)&eject, &length);
        if (!NT_SUCCESS(status))
        {
            KdPrint(("WdfRequestRetrieveInputBuffer failed 0x%x\n", status));
            break;
        }

        if (eject->Size == InputBufferLength)
        {
            status = BusEjectDevice(hDevice, eject->SerialNo);
        }

        break;

    default:
        break;
    }
    WdfRequestCompleteWithInformation(Request, status, length);
}
