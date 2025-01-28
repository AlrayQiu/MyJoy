#include "hid_def/default/myjoy_default_hid_def.h"
#include "queue.h"
#include "request.h"

#include "utils.h"
#include "wdftimer.h"

#include "context.h"

void EvtTimerFunc(IN WDFTIMER Timer);

NTSTATUS ManualQueueCreate(IN WDFDEVICE Device, OUT WDFQUEUE *Queue)
{
    KdPrint(("ManualQueueCreate Start\r\n"));
    NTSTATUS              status;
    WDF_IO_QUEUE_CONFIG   queueConfig;
    WDF_OBJECT_ATTRIBUTES queueAttributes;
    WDFQUEUE              queue;
    PMANUAL_QUEUE_CONTEXT queueContext;
    WDF_TIMER_CONFIG      timerConfig;
    WDF_OBJECT_ATTRIBUTES timerAttributes;
    ULONG                 timerPeriodInSeconds = 5;

    WDF_IO_QUEUE_CONFIG_INIT(&queueConfig, WdfIoQueueDispatchManual);

    WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&queueAttributes, MANUAL_QUEUE_CONTEXT);

    status = WdfIoQueueCreate(Device, &queueConfig, &queueAttributes, &queue);

    if (!NT_SUCCESS(status))
    {
        KdPrint(("WdfIoQueueCreate failed 0x%x\n", status));
        return status;
    }

    queueContext                = GetManualQueueContext(queue);
    queueContext->Queue         = queue;
    queueContext->DeviceContext = GetDeviceContext(Device);

    WDF_TIMER_CONFIG_INIT_PERIODIC(&timerConfig, EvtTimerFunc, timerPeriodInSeconds * 1000);

    WDF_OBJECT_ATTRIBUTES_INIT(&timerAttributes);
    timerAttributes.ParentObject = queue;
    status                       = WdfTimerCreate(&timerConfig, &timerAttributes, &queueContext->Timer);
    ASSERT_STATUS_SUCCESS(status, "WdfTimerCreate Failed");

    LONGLONG timeout = WDF_REL_TIMEOUT_IN_SEC(1);
    WdfTimerStart(queueContext->Timer, timeout);

    *Queue = queue;

    KdPrint(("ManualQueueCreate End:0x%x\r\n", status));
    return status;
}

void EvtTimerFunc(IN WDFTIMER Timer)
{
    NTSTATUS              status;
    WDFQUEUE              queue;
    PMANUAL_QUEUE_CONTEXT queueContext;
    WDFREQUEST            request;
    HID_INPUT_REPORT      readReport;

    queue        = (WDFQUEUE)WdfTimerGetParentObject(Timer);
    queueContext = GetManualQueueContext(queue);

    //
    // see if we have a request in manual queue
    //
    status = WdfIoQueueRetrieveNextRequest(queueContext->Queue, &request);

    if (NT_SUCCESS(status))
    {
        KdPrint(("WdfIoQueueRetrieveNextRequest\n"));

        readReport = queueContext->DeviceContext->DeviceData;

        status = RequestCopyFromBuffer(request, &readReport, sizeof(readReport));

        WdfRequestComplete(request, status);
    }
}
