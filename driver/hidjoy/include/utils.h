#pragma once
// clang-format off
#define ASSERT_STATUS_SUCCESS(status, message, ...) \
    if (!NT_SUCCESS(status))                        \
    {                                               \
        KdPrint(("Error: "));                       \
        KdPrint((message));                         \
        KdPrint(("Status = 0x%x\r\n", status));     \
        return status;                              \
    }

#define ASSERT_STATUS_IS(status, isStatus, message, ...) \
    if (status == isStatus)                              \
    {                                                    \
        KdPrint((message));                              \
        KdPrint(("\r\n"));                               \
        return status;                                   \
    }
// clang-format on