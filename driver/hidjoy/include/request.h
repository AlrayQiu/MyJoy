#pragma once

#include <ntddk.h>
#include <wdf.h>

#include <minwindef.h>

#include <hidclass.h>
#include "wdftypes.h"

NTSTATUS RequestCopyFromBuffer(IN WDFREQUEST Request,
                               IN PVOID      SourceBuffer,
                               _When_(NumBytesToCopyFrom == 0,
                                      __drv_reportError(NumBytesToCopyFrom cannot be zero))
                                   IN size_t NumBytesToCopyFrom);

NTSTATUS RequestGetHidXferPacket_ToReadFromDevice(IN WDFREQUEST Request, OUT HID_XFER_PACKET *Packet);

NTSTATUS RequestGetHidXferPacket_ToWriteToDevice(IN WDFREQUEST Request, OUT HID_XFER_PACKET *Packet);
