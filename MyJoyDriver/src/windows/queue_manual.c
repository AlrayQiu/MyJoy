
#include "queue.h"

#include "context.h"

NTSTATUS ManualQueueCreate(IN WDFDEVICE Device, OUT WDFQUEUE *Queue)
{
    NTSTATUS              status;
    WDF_IO_QUEUE_CONFIG   queueConfig;
    WDF_OBJECT_ATTRIBUTES queueAttributes;
    WDFQUEUE              queue;
    PMANUAL_QUEUE_CONTEXT queueContext;
    WDF_TIMER_CONFIG      timerConfig;
    WDF_OBJECT_ATTRIBUTES timerAttributes;
    ULONG                 timerPeriodInSeconds = 5;

    WDF_IO_QUEUE_CONFIG_INIT(&queueConfig, WdfIoQueueDispatchManual);

    WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&queueAttributes,
                                            MANUAL_QUEUE_CONTEXT);

    status = WdfIoQueueCreate(Device, &queueConfig, &queueAttributes, &queue);

    if (!NT_SUCCESS(status))
    {
        KdPrint(("WdfIoQueueCreate failed 0x%x\n", status));
        return status;
    }

    queueContext                = GetManualQueueContext(queue);
    queueContext->Queue         = queue;
    queueContext->DeviceContext = GetDeviceContext(Device);

    WDF_OBJECT_ATTRIBUTES_INIT(&timerAttributes);
    timerAttributes.ParentObject = queue;

    *Queue = queue;

    return status;
}
