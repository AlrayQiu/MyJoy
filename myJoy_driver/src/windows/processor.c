#include "processor.h"
#include "default/myjoy_default_hid_def.h"
#include "hid_def.h"
#include "request.h"

NTSTATUS
ReadReport(IN PQUEUE_CONTEXT QueueContext, IN WDFREQUEST Request, _Always_(OUT) BOOLEAN *CompleteRequest)
{
    NTSTATUS         status;
    HID_INPUT_REPORT readReport;

    KdPrint(("ReadReport\n"));

    status = WdfIoQueueRetrieveNextRequest(QueueContext->Queue, &Request);

    if (NT_SUCCESS(status))
    {
        readReport = QueueContext->DeviceContext->DeviceData;

        status = RequestCopyFromBuffer(Request, &readReport, sizeof(readReport));
    }

    if (!NT_SUCCESS(status))
    {
        KdPrint(("WdfRequestForwardToIoQueue failed with 0x%x\n", status));
    }

    *CompleteRequest = TRUE;

    return status;
}

NTSTATUS WriteReport(_In_ PQUEUE_CONTEXT QueueContext, _In_ WDFREQUEST Request)
{
    NTSTATUS        status;
    ULONG           reportSize;
    PHID_OUT_REPORT outputReport;
    KdPrint(("WriteReport\n"));
    status =
        WdfRequestRetrieveInputBuffer(Request, sizeof(HID_OUT_REPORT), (PVOID *)&outputReport, NULL);
    if (NT_SUCCESS(status))
    {
        QueueContext->DeviceContext->DeviceData = *outputReport;
    }
    WdfRequestComplete(Request, status);
    return status;
}

NTSTATUS SetFeature(IN PQUEUE_CONTEXT QueueContext, IN WDFREQUEST Request)
{
    NTSTATUS               status;
    HID_XFER_PACKET        packet;
    ULONG                  reportSize;
    PMYJOY_CONTROL_INFO    controlInfo;
    PHID_DEVICE_ATTRIBUTES hidAttributes = &QueueContext->DeviceContext->HidDeviceAttributes;

    status = RequestGetHidXferPacket_ToWriteToDevice(Request, &packet);
    if (!NT_SUCCESS(status))
    {
        return status;
    }

    if (packet.reportId != CONTROL_COLLECTION_REPORT_ID)
    {
        status = STATUS_INVALID_PARAMETER;
        KdPrint(("SetFeature: invalid report id %d\n", packet.reportId));
        return status;
    }

    reportSize = sizeof(HIDMINI_CONTROL_INFO);

    if (packet.reportBufferLen < reportSize)
    {
        status = STATUS_INVALID_BUFFER_SIZE;
        KdPrint(("SetFeature: invalid input buffer. size %d, expect %d\n",
                 packet.reportBufferLen,
                 reportSize));
        return status;
    }

    controlInfo = (PMYJOY_CONTROL_INFO)packet.reportBuffer;

    switch (controlInfo->ControlCode)
    {
    case HIDMINI_CONTROL_CODE_SET_ATTRIBUTES:

        hidAttributes->ProductID     = controlInfo->u.Attributes.ProductID;
        hidAttributes->VendorID      = controlInfo->u.Attributes.VendorID;
        hidAttributes->VersionNumber = controlInfo->u.Attributes.VersionNumber;

        WdfRequestSetInformation(Request, reportSize);
        break;

    case HIDMINI_CONTROL_CODE_DUMMY1:
        status = STATUS_NOT_IMPLEMENTED;
        KdPrint(("SetFeature: HIDMINI_CONTROL_CODE_DUMMY1\n"));
        break;

    case HIDMINI_CONTROL_CODE_DUMMY2:
        status = STATUS_NOT_IMPLEMENTED;
        KdPrint(("SetFeature: HIDMINI_CONTROL_CODE_DUMMY2\n"));
        break;

    default:
        status = STATUS_NOT_IMPLEMENTED;
        KdPrint(("SetFeature: Unknown control Code 0x%x\n", controlInfo->ControlCode));
        break;
    }

    return status;
}

NTSTATUS GetFeature(IN PQUEUE_CONTEXT QueueContext, IN WDFREQUEST Request)
{
    NTSTATUS               status;
    HID_XFER_PACKET        packet;
    ULONG                  reportSize;
    PMY_DEVICE_ATTRIBUTES  myAttributes;
    PHID_DEVICE_ATTRIBUTES hidAttributes = &QueueContext->DeviceContext->HidDeviceAttributes;

    KdPrint(("GetFeature\n"));

    status = RequestGetHidXferPacket_ToReadFromDevice(Request, &packet);
    if (!NT_SUCCESS(status))
    {
        return status;
    }

    if (packet.reportId != CONTROL_COLLECTION_REPORT_ID)
    {
        status = STATUS_INVALID_PARAMETER;
        KdPrint(("GetFeature: invalid report id %d\n", packet.reportId));
        return status;
    }

    reportSize = sizeof(MY_DEVICE_ATTRIBUTES) + sizeof(packet.reportId);
    if (packet.reportBufferLen < reportSize)
    {
        status = STATUS_INVALID_BUFFER_SIZE;
        KdPrint(("GetFeature: output buffer too small. Size %d, expect %d\n",
                 packet.reportBufferLen,
                 reportSize));
        return status;
    }

    myAttributes                = (PMY_DEVICE_ATTRIBUTES)(packet.reportBuffer + sizeof(packet.reportId));
    myAttributes->ProductID     = hidAttributes->ProductID;
    myAttributes->VendorID      = hidAttributes->VendorID;
    myAttributes->VersionNumber = hidAttributes->VersionNumber;

    WdfRequestSetInformation(Request, reportSize);
    return status;
}
NTSTATUS GetInputReport(IN PQUEUE_CONTEXT QueueContext, IN WDFREQUEST Request)
{
    NTSTATUS          status;
    HID_XFER_PACKET   packet;
    ULONG             reportSize;
    PHID_INPUT_REPORT reportBuffer;

    KdPrint(("GetInputReport\n"));

    status = RequestGetHidXferPacket_ToReadFromDevice(Request, &packet);
    if (!NT_SUCCESS(status))
    {
        return status;
    }

    if (packet.reportId != CONTROL_COLLECTION_REPORT_ID)
    {
        status = STATUS_INVALID_PARAMETER;
        KdPrint(("GetInputReport: invalid report id %d\n", packet.reportId));
        return status;
    }

    reportSize = sizeof(HID_INPUT_REPORT);
    if (packet.reportBufferLen < reportSize)
    {
        status = STATUS_INVALID_BUFFER_SIZE;
        KdPrint(("GetInputReport: output buffer too small. Size %d, expect %d\n",
                 packet.reportBufferLen,
                 reportSize));
        return status;
    }

    reportBuffer = (PHID_INPUT_REPORT)(packet.reportBuffer);

    *reportBuffer = QueueContext->DeviceContext->DeviceData;

    WdfRequestSetInformation(Request, reportSize);
    return status;
}

NTSTATUS SetOutputReport(IN PQUEUE_CONTEXT QueueContext, IN WDFREQUEST Request)
{
    NTSTATUS        status;
    HID_XFER_PACKET packet;
    ULONG           reportSize;
    PHID_OUT_REPORT reportBuffer;

    KdPrint(("SetOutputReport\n"));

    status = RequestGetHidXferPacket_ToWriteToDevice(Request, &packet);
    if (!NT_SUCCESS(status))
    {
        return status;
    }

    if (packet.reportId != CONTROL_COLLECTION_REPORT_ID)
    {
        status = STATUS_INVALID_PARAMETER;
        KdPrint(("SetOutputReport: unkown report id %d\n", packet.reportId));
        return status;
    }

    reportSize = sizeof(HID_OUT_REPORT);

    if (packet.reportBufferLen < reportSize)
    {
        status = STATUS_INVALID_BUFFER_SIZE;
        KdPrint(("SetOutputReport: invalid input buffer. size %d, expect %d\n",
                 packet.reportBufferLen,
                 reportSize));
        return status;
    }

    reportBuffer = (PHID_OUT_REPORT)packet.reportBuffer;

    QueueContext->DeviceContext->DeviceData = *reportBuffer;

    WdfRequestSetInformation(Request, reportSize);
    return status;
}

NTSTATUS GetIndexedString(_In_ WDFREQUEST Request)
{
    NTSTATUS status;
    ULONG    languageId, stringIndex;

    status = GetStringId(Request, &stringIndex, &languageId);

    // While we don't use the language id, some minidrivers might.
    //
    UNREFERENCED_PARAMETER(languageId);

    if (NT_SUCCESS(status))
    {
        if (stringIndex != MYJOY_DEVICE_STRING_INDEX)
        {
            status = STATUS_INVALID_PARAMETER;
            KdPrint(("GetString: unkown string index %d\n", stringIndex));
            return status;
        }

        status = RequestCopyFromBuffer(Request, MYJOY_DEVICE_STRING, sizeof(MYJOY_DEVICE_STRING));
    }
    return status;
}

NTSTATUS
GetString(IN WDFREQUEST Request)
{
    NTSTATUS status;
    ULONG    languageId, stringId;
    size_t   stringSizeCb;
    PWSTR    string;

    status = GetStringId(Request, &stringId, &languageId);

    UNREFERENCED_PARAMETER(languageId);

    if (!NT_SUCCESS(status))
    {
        return status;
    }

    switch (stringId)
    {
    case HID_STRING_ID_IMANUFACTURER:
        stringSizeCb = sizeof(MYJOY_MANUFACTURER_STRING);
        string       = MYJOY_MANUFACTURER_STRING;
        break;
    case HID_STRING_ID_IPRODUCT:
        stringSizeCb = sizeof(MYJOY_PRODUCT_STRING);
        string       = MYJOY_PRODUCT_STRING;
        break;
    case HID_STRING_ID_ISERIALNUMBER:
        stringSizeCb = sizeof(MYJOY_SERIAL_NUMBER_STRING);
        string       = MYJOY_SERIAL_NUMBER_STRING;
        break;
    default:
        status = STATUS_INVALID_PARAMETER;
        KdPrint(("GetString: unkown string id %d\n", stringId));
        return status;
    }

    status = RequestCopyFromBuffer(Request, string, stringSizeCb);
    return status;
}

NTSTATUS GetStringId(_In_ WDFREQUEST Request, _Out_ ULONG *StringId, _Out_ ULONG *LanguageId)
{
    NTSTATUS status;
    ULONG    inputValue;

    WDFMEMORY inputMemory;
    size_t    inputBufferLength;
    PVOID     inputBuffer;

    WDF_REQUEST_PARAMETERS requestParameters;

    WDF_REQUEST_PARAMETERS_INIT(&requestParameters);
    WdfRequestGetParameters(Request, &requestParameters);

    inputValue = PtrToUlong(requestParameters.Parameters.DeviceIoControl.Type3InputBuffer);

    status = STATUS_SUCCESS;

    *StringId = (inputValue & 0x0ffff);

    *LanguageId = (inputValue >> 16);

    return status;
}