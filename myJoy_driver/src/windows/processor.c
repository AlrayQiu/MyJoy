#include "processor.h"
#include "WudfWdm.h"
#include "default/myjoy_default_hid_def.h"
#include "hid_def.h"
#include "request.h"

#include <ntstatus.h>
#include <winnt.h>

NTSTATUS ReadReport(IN PQUEUE_CONTEXT      QueueContext,
                    IN WDFREQUEST          Request,
                    _Always_(OUT) BOOLEAN *CompleteRequest)
{
    NTSTATUS         status;
    HID_INPUT_REPORT readReport;

    KdPrint(("ReadReport\n"));

    status = WdfIoQueueRetrieveNextRequest(QueueContext->Queue, &Request);

    if (NT_SUCCESS(status))
    {
        readReport = QueueContext->DeviceContext->DeviceData;

        status =
            RequestCopyFromBuffer(Request, &readReport, sizeof(readReport));
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
    status = WdfRequestRetrieveInputBuffer(
        Request, sizeof(HID_OUT_REPORT), (PVOID *)&outputReport, NULL);
    if (NT_SUCCESS(status))
    {
        QueueContext->DeviceContext->DeviceData = *outputReport;
    }
    WdfRequestComplete(Request, status);
    return status;
}

NTSTATUS SetFeature(IN PQUEUE_CONTEXT QueueContext, IN WDFREQUEST Request)
{
    return STATUS_NOINTERFACE;
}

NTSTATUS GetFeature(IN PQUEUE_CONTEXT QueueContext, IN WDFREQUEST Request)
{
    return STATUS_NOINTERFACE;
}
NTSTATUS GetInputReport(IN PQUEUE_CONTEXT QueueContext, IN WDFREQUEST Request)
{
    return STATUS_NOINTERFACE;
}

NTSTATUS SetOutputReport(IN PQUEUE_CONTEXT QueueContext, IN WDFREQUEST Request)
{
    return STATUS_NOINTERFACE;
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

        status = RequestCopyFromBuffer(
            Request, MYJOY_DEVICE_STRING, sizeof(MYJOY_DEVICE_STRING));
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

NTSTATUS GetStringId(_In_ WDFREQUEST Request,
                     _Out_ ULONG    *StringId,
                     _Out_ ULONG    *LanguageId)
{
    NTSTATUS status;
    ULONG    inputValue;

    WDFMEMORY inputMemory;
    size_t    inputBufferLength;
    PVOID     inputBuffer;

    status = WdfRequestRetrieveInputMemory(Request, &inputMemory);
    if (!NT_SUCCESS(status))
    {
        KdPrint(("WdfRequestRetrieveInputMemory failed 0x%x\n", status));
        return status;
    }
    inputBuffer = WdfMemoryGetBuffer(inputMemory, &inputBufferLength);

    if (inputBufferLength < sizeof(ULONG))
    {
        status = STATUS_INVALID_BUFFER_SIZE;
        KdPrint(("GetStringId: invalid input buffer. size %d, expect %d\n",
                 (int)inputBufferLength,
                 (int)sizeof(ULONG)));
        return status;
    }

    inputValue = (*(PULONG)inputBuffer);

    *StringId = (inputValue & 0x0ffff);

    *LanguageId = (inputValue >> 16);

    return status;
}