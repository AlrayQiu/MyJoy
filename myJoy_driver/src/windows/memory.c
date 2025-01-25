
#include "request.h"

#include <windows.h>
#include <wdf.h>
#include <wdfmemory.h>

NTSTATUS RequestCopyFromBuffer(
    IN WDFREQUEST Request,
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
        KdPrint(
            ("RequestCopyFromBuffer: buffer too small. Size %d, expect %d\n",
             (int)outputBufferLength,
             (int)NumBytesToCopyFrom));
        return status;
    }

    status =
        WdfMemoryCopyFromBuffer(memory, 0, SourceBuffer, NumBytesToCopyFrom);
    if (!NT_SUCCESS(status))
    {
        KdPrint(("WdfMemoryCopyFromBuffer failed 0x%x\n", status));
        return status;
    }

    WdfRequestSetInformation(Request, NumBytesToCopyFrom);
    return status;
}
