#include "request.h"

NTSTATUS RequestCopyFromBuffer(
    IN WDFREQUEST Request,
    IN PVOID      SourceBuffer,
    _When_(NumBytesToCopyFrom == 0,
           __drv_reportError(NumBytesToCopyFrom cannot be zero))
        IN size_t NumBytesToCopyFrom)
{
}
