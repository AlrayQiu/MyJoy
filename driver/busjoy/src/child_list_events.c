#include "child.h"

#ifdef ALLOC_PRAGMA
#pragma alloc_text(PAGE, BusPlugInDevice)
#pragma alloc_text(PAGE, BusUnPlugDevice)
#pragma alloc_text(PAGE, BusEjectDevice)
#endif

NTSTATUS BusPlugInDevice(WDFDEVICE Device, PWCHAR HardwareIds, size_t CchHardwareIds, ULONG SerialNo)
{
    CHILD_PDO_IDENTIFICATION_DESCRIPTION description;
    NTSTATUS                             status = STATUS_SUCCESS;

    PAGED_CODE();

    WDF_CHILD_IDENTIFICATION_DESCRIPTION_HEADER_INIT(&description.Header, sizeof(description));

    description.SerialNo = SerialNo;

    status =
        WdfChildListAddOrUpdateChildDescriptionAsPresent(WdfFdoGetDefaultChildList(Device), &description.Header, NULL);

    return status;
}

NTSTATUS BusUnPlugDevice(WDFDEVICE Device, ULONG SerialNo)
{
    NTSTATUS     status;
    WDFCHILDLIST list;

    PAGED_CODE();

    list = WdfFdoGetDefaultChildList(Device);

    if (0 == SerialNo)
    {
        status = STATUS_SUCCESS;

        WdfChildListBeginScan(list);
        WdfChildListEndScan(list);
    }
    else
    {
        CHILD_PDO_IDENTIFICATION_DESCRIPTION description;

        WDF_CHILD_IDENTIFICATION_DESCRIPTION_HEADER_INIT(&description.Header, sizeof(description));

        description.SerialNo = SerialNo;

        status = WdfChildListUpdateChildDescriptionAsMissing(list, &description.Header);
    }

    return status;
}

NTSTATUS
BusEjectDevice(WDFDEVICE Device, ULONG SerialNo)
{
    WDFDEVICE    hChild;
    NTSTATUS     status = STATUS_INVALID_PARAMETER;
    WDFCHILDLIST list;

    PAGED_CODE();

    list = WdfFdoGetDefaultChildList(Device);

    if (0 == SerialNo)
    {
        WDF_CHILD_LIST_ITERATOR iterator;

        WDF_CHILD_LIST_ITERATOR_INIT(&iterator, WdfRetrievePresentChildren);

        WdfChildListBeginIteration(list, &iterator);

        for (;;)
        {
            WDF_CHILD_RETRIEVE_INFO              childInfo;
            CHILD_PDO_IDENTIFICATION_DESCRIPTION description;
            BOOLEAN                              ret;

            WDF_CHILD_IDENTIFICATION_DESCRIPTION_HEADER_INIT(&description.Header, sizeof(description));
            WDF_CHILD_RETRIEVE_INFO_INIT(&childInfo, &description.Header);

            WDF_CHILD_IDENTIFICATION_DESCRIPTION_HEADER_INIT(&description.Header, sizeof(description));

            status = WdfChildListRetrieveNextDevice(list, &iterator, &hChild, &childInfo);

            if (!NT_SUCCESS(status) || status == STATUS_NO_MORE_ENTRIES)
            {
                break;
            }

            ASSERT(childInfo.Status == WdfChildListRetrieveDeviceSuccess);

            ret = WdfChildListRequestChildEject(list, &description.Header);
            if (!ret)
            {
                WDFVERIFY(ret);
            }
        }

        WdfChildListEndIteration(list, &iterator);

        if (status == STATUS_NO_MORE_ENTRIES)
        {
            status = STATUS_SUCCESS;
        }
    }
    else
    {
        CHILD_PDO_IDENTIFICATION_DESCRIPTION description;

        WDF_CHILD_IDENTIFICATION_DESCRIPTION_HEADER_INIT(&description.Header, sizeof(description));

        description.SerialNo = SerialNo;

        if (WdfChildListRequestChildEject(list, &description.Header))
        {
            status = STATUS_SUCCESS;
        }
    }

    return status;
}
