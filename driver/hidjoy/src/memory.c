
#include "request.h"

#include <ntddk.h>
#include <wdf.h>
#include <wdfmemory.h>

NTSTATUS RequestCopyFromBuffer(IN WDFREQUEST Request,
                               IN PVOID      SourceBuffer,
                               _When_(NumBytesToCopyFrom == 0,
                                      __drv_reportError(NumBytesToCopyFrom cannot be zero))
                                   IN size_t NumBytesToCopyFrom)
{
    NTSTATUS  status;
    WDFMEMORY memory;
    size_t    outputBufferLength;

    status = WdfRequestRetrieveOutputMemory(Request, &memory);
    if (!NT_SUCCESS(status))
    {
        KdPrint(("WdfRequestRetrieveOutputMemory failed 0x%x\n", status));
        return status;
    }

    WdfMemoryGetBuffer(memory, &outputBufferLength);
    if (outputBufferLength < NumBytesToCopyFrom)
    {
        status = STATUS_INVALID_BUFFER_SIZE;
        KdPrint(("RequestCopyFromBuffer: buffer too small. Size %d, expect %d\n",
                 (int)outputBufferLength,
                 (int)NumBytesToCopyFrom));
        return status;
    }

    status = WdfMemoryCopyFromBuffer(memory, 0, SourceBuffer, NumBytesToCopyFrom);
    if (!NT_SUCCESS(status))
    {
        KdPrint(("WdfMemoryCopyFromBuffer failed 0x%x\n", status));
        return status;
    }

    WdfRequestSetInformation(Request, NumBytesToCopyFrom);
    return status;
}

NTSTATUS
RequestGetHidXferPacket_ToReadFromDevice(_In_ WDFREQUEST Request, _Out_ HID_XFER_PACKET *Packet)
{
    NTSTATUS               status;
    WDF_REQUEST_PARAMETERS params;

    WDF_REQUEST_PARAMETERS_INIT(&params);
    WdfRequestGetParameters(Request, &params);

    if (params.Parameters.DeviceIoControl.OutputBufferLength < sizeof(HID_XFER_PACKET))
    {
        status = STATUS_BUFFER_TOO_SMALL;
        KdPrint(("RequestGetHidXferPacket: invalid HID_XFER_PACKET\n"));
        return status;
    }

    RtlCopyMemory(Packet, WdfRequestWdmGetIrp(Request)->UserBuffer, sizeof(HID_XFER_PACKET));
    return STATUS_SUCCESS;
}

NTSTATUS
RequestGetHidXferPacket_ToWriteToDevice(_In_ WDFREQUEST Request, _Out_ HID_XFER_PACKET *Packet)
{
    NTSTATUS               status;
    WDF_REQUEST_PARAMETERS params;

    WDF_REQUEST_PARAMETERS_INIT(&params);
    WdfRequestGetParameters(Request, &params);

    if (params.Parameters.DeviceIoControl.InputBufferLength < sizeof(HID_XFER_PACKET))
    {
        status = STATUS_BUFFER_TOO_SMALL;
        KdPrint(("RequestGetHidXferPacket: invalid HID_XFER_PACKET\n"));
        return status;
    }

    RtlCopyMemory(Packet, WdfRequestWdmGetIrp(Request)->UserBuffer, sizeof(HID_XFER_PACKET));
    return STATUS_SUCCESS;
}