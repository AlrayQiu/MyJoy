#pragma once

#define ASSERT_STATUS_SUCCESS(status, message, ...) \
    if (!NT_SUCCESS(status))                        \
    {                                               \
        KdPrint(("Error:" message, ##__VA_ARGS__)); \
        KdPrint(("Status = 0x%x\n", status));       \
        return status;                              \
    }

#define ASSERT_STATUS_IS(status, isStatus, message, ...) \
    if (status == isStatus)                              \
    {                                                    \
        KdPrint((message "\n", ##__VA_ARGS__));          \
        return status;                                   \
    }