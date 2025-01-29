#include "bus_public.h"
#include "ntddk.h"
#include <child.h>

#include <ntintsafe.h>
#include "ntstrsafe.h"

ULONG G_BusJoyDebugLevel;

NTSTATUS BusCreatePdo(_In_ WDFDEVICE                         Device,
                      _In_ PWDFDEVICE_INIT                   DeviceInit,
                      _In_reads_(MAX_INSTANCE_ID_LEN) PCWSTR HardwareIds,
                      _In_ ULONG                             SerialNo);

NTSTATUS BusEvtDeviceListCreatePdo(WDFCHILDLIST                                 DeviceList,
                                   PWDF_CHILD_IDENTIFICATION_DESCRIPTION_HEADER IdentificationDescription,
                                   PWDFDEVICE_INIT                              ChildInit);

#ifdef ALLOC_PRAGMA
#pragma alloc_text(PAGE, BusCreatePdo)
#pragma alloc_text(PAGE, BusEvtDeviceListCreatePdo)
#endif

NTSTATUS BusEvtChildListIdentificationDescriptionDuplicate(
    WDFCHILDLIST                                 DeviceList,
    PWDF_CHILD_IDENTIFICATION_DESCRIPTION_HEADER SourceIdentificationDescription,
    PWDF_CHILD_IDENTIFICATION_DESCRIPTION_HEADER DestinationIdentificationDescription)

{
    KdPrint(("BusEvtChildListIdentificationDescriptionDuplicate\r\n"));
    PCHILD_PDO_IDENTIFICATION_DESCRIPTION src, dst;
    size_t                                safeMultResult;
    NTSTATUS                              status;

    UNREFERENCED_PARAMETER(DeviceList);

    src = CONTAINING_RECORD(SourceIdentificationDescription, CHILD_PDO_IDENTIFICATION_DESCRIPTION, Header);
    dst = CONTAINING_RECORD(DestinationIdentificationDescription, CHILD_PDO_IDENTIFICATION_DESCRIPTION, Header);

    dst->SerialNo = src->SerialNo;

    return STATUS_SUCCESS;
}

BOOLEAN BusEvtChildListIdentificationDescriptionCompare(
    WDFCHILDLIST                                 DeviceList,
    PWDF_CHILD_IDENTIFICATION_DESCRIPTION_HEADER FirstIdentificationDescription,
    PWDF_CHILD_IDENTIFICATION_DESCRIPTION_HEADER SecondIdentificationDescription)

{
    KdPrint(("BusEvtChildListIdentificationDescriptionCompare"));
    PCHILD_PDO_IDENTIFICATION_DESCRIPTION lhs, rhs;

    UNREFERENCED_PARAMETER(DeviceList);

    lhs = CONTAINING_RECORD(FirstIdentificationDescription, CHILD_PDO_IDENTIFICATION_DESCRIPTION, Header);
    rhs = CONTAINING_RECORD(SecondIdentificationDescription, CHILD_PDO_IDENTIFICATION_DESCRIPTION, Header);

    return (lhs->SerialNo == rhs->SerialNo) ? TRUE : FALSE;
}

#pragma prefast(push)
#pragma prefast(disable : 6101, "No need to assign IdentificationDescription")

VOID BusEvtChildListIdentificationDescriptionCleanup(
    _In_ WDFCHILDLIST DeviceList, _Out_ PWDF_CHILD_IDENTIFICATION_DESCRIPTION_HEADER IdentificationDescription)
{
}

#pragma prefast(pop) // disable:6101

NTSTATUS BusEvtDeviceListCreatePdo(WDFCHILDLIST                                 DeviceList,
                                   PWDF_CHILD_IDENTIFICATION_DESCRIPTION_HEADER IdentificationDescription,
                                   PWDFDEVICE_INIT                              ChildInit)
{
    PCHILD_PDO_IDENTIFICATION_DESCRIPTION pDesc;

    PAGED_CODE();

    pDesc = CONTAINING_RECORD(IdentificationDescription, CHILD_PDO_IDENTIFICATION_DESCRIPTION, Header);

    return BusCreatePdo(WdfChildListGetDevice(DeviceList), ChildInit, BUS_HARDWARE_IDS, pDesc->SerialNo);
}

NTSTATUS BusCreatePdo(_In_ WDFDEVICE                         Device,
                      _In_ PWDFDEVICE_INIT                   DeviceInit,
                      _In_reads_(MAX_INSTANCE_ID_LEN) PCWSTR HardwareIds,
                      _In_ ULONG                             SerialNo)
{
    NTSTATUS                      status;
    PPDO_DEVICE_DATA              pdoData = NULL;
    WDFDEVICE                     hChild  = NULL;
    WDF_QUERY_INTERFACE_CONFIG    qiConfig;
    WDF_OBJECT_ATTRIBUTES         pdoAttributes;
    WDF_DEVICE_PNP_CAPABILITIES   pnpCaps;
    WDF_DEVICE_POWER_CAPABILITIES powerCaps;
    INTERFACE                     BusJoyInterface;
    DECLARE_CONST_UNICODE_STRING(compatId, BUSENUM_COMPATIBLE_IDS);
    DECLARE_CONST_UNICODE_STRING(deviceLocation, L"Bus Joy 0");
    DECLARE_UNICODE_STRING_SIZE(buffer, MAX_INSTANCE_ID_LEN);
    DECLARE_UNICODE_STRING_SIZE(deviceId, MAX_INSTANCE_ID_LEN);

    PAGED_CODE();

    UNREFERENCED_PARAMETER(Device);

    KdPrint(("Entered Bus_CreatePdo\n"));

    WdfDeviceInitSetDeviceType(DeviceInit, FILE_DEVICE_BUS_EXTENDER);

    RtlInitUnicodeString(&deviceId, HardwareIds);

    status = WdfPdoInitAssignDeviceID(DeviceInit, &deviceId);
    if (!NT_SUCCESS(status))
    {
        return status;
    }

    status = WdfPdoInitAddHardwareID(DeviceInit, &deviceId);
    if (!NT_SUCCESS(status))
    {
        return status;
    }

    status = WdfPdoInitAddCompatibleID(DeviceInit, &compatId);
    if (!NT_SUCCESS(status))
    {
        return status;
    }

    status = RtlUnicodeStringPrintf(&buffer, L"%02d", SerialNo);
    if (!NT_SUCCESS(status))
    {
        return status;
    }

    status = WdfPdoInitAssignInstanceID(DeviceInit, &buffer);
    if (!NT_SUCCESS(status))
    {
        return status;
    }

    status = RtlUnicodeStringPrintf(&buffer, L"Myjoy_BusJoy_Device %02d", SerialNo);
    if (!NT_SUCCESS(status))
    {
        return status;
    }

    status = WdfPdoInitAddDeviceText(DeviceInit, &buffer, &deviceLocation, 0x409);
    if (!NT_SUCCESS(status))
    {
        return status;
    }

    WdfPdoInitSetDefaultLocale(DeviceInit, 0x409);

    WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&pdoAttributes, PDO_DEVICE_DATA);

    status = WdfDeviceCreate(&DeviceInit, &pdoAttributes, &hChild);
    if (!NT_SUCCESS(status))
    {
        return status;
    }

    pdoData = GetPdoContext(hChild);

    pdoData->SerialNo = SerialNo;

    WDF_DEVICE_PNP_CAPABILITIES_INIT(&pnpCaps);
    pnpCaps.Removable         = WdfTrue;
    pnpCaps.EjectSupported    = WdfTrue;
    pnpCaps.SurpriseRemovalOK = WdfTrue;

    pnpCaps.Address  = SerialNo;
    pnpCaps.UINumber = SerialNo;

    WdfDeviceSetPnpCapabilities(hChild, &pnpCaps);

    WDF_DEVICE_POWER_CAPABILITIES_INIT(&powerCaps);

    powerCaps.DeviceD1   = WdfTrue;
    powerCaps.WakeFromD1 = WdfTrue;
    powerCaps.DeviceWake = PowerDeviceD1;

    powerCaps.DeviceState[PowerSystemWorking]   = PowerDeviceD1;
    powerCaps.DeviceState[PowerSystemSleeping1] = PowerDeviceD1;
    powerCaps.DeviceState[PowerSystemSleeping2] = PowerDeviceD2;
    powerCaps.DeviceState[PowerSystemSleeping3] = PowerDeviceD2;
    powerCaps.DeviceState[PowerSystemHibernate] = PowerDeviceD3;
    powerCaps.DeviceState[PowerSystemShutdown]  = PowerDeviceD3;

    WdfDeviceSetPowerCapabilities(hChild, &powerCaps);

    RtlZeroMemory(&BusJoyInterface, sizeof(BusJoyInterface));

    BusJoyInterface.Size    = sizeof(BusJoyInterface);
    BusJoyInterface.Version = 1;
    BusJoyInterface.Context = (PVOID)hChild;

    BusJoyInterface.InterfaceReference   = WdfDeviceInterfaceReferenceNoOp;
    BusJoyInterface.InterfaceDereference = WdfDeviceInterfaceDereferenceNoOp;

    WDF_QUERY_INTERFACE_CONFIG_INIT(&qiConfig, (PINTERFACE)&BusJoyInterface, &GUID_DEVINTERFACE_BUSJOY, NULL);

    status = WdfDeviceAddQueryInterface(hChild, &qiConfig);

    if (!NT_SUCCESS(status))
    {
        return status;
    }

    return status;
}
