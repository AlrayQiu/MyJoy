#include "queue.h"
#include "request.h"
#include "ioprocessor.h"

#include "context.h"
#include "utils.h"

EVT_WDF_IO_QUEUE_IO_DEVICE_CONTROL EvtIoDeviceControl;

NTSTATUS QueueCreate(IN WDFDEVICE Device, OUT WDFQUEUE *Queue)

{
    NTSTATUS              status;
    WDF_IO_QUEUE_CONFIG   queueConfig;
    WDF_OBJECT_ATTRIBUTES queueAttributes;
    WDFQUEUE              queue;
    PQUEUE_CONTEXT        queueContext;

    WDF_IO_QUEUE_CONFIG_INIT_DEFAULT_QUEUE(&queueConfig,
                                           WdfIoQueueDispatchParallel);

    queueConfig.EvtIoDeviceControl = EvtIoDeviceControl;

    WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&queueAttributes, QUEUE_CONTEXT);

    status = WdfIoQueueCreate(Device, &queueConfig, &queueAttributes, &queue);
    ASSERT_STATUS_SUCCESS(status, "WdfIoQueueCreate failed ");

    queueContext                = GetQueueContext(queue);
    queueContext->Queue         = queue;
    queueContext->DeviceContext = GetDeviceContext(Device);
    queueContext->OutputReport  = 0;

    *Queue = queue;
    return status;
}

VOID EvtIoDeviceControl(IN WDFQUEUE   Queue,
                        IN WDFREQUEST Request,
                        IN size_t     OutputBufferLength,
                        IN size_t     InputBufferLength,
                        IN ULONG      IoControlCode)
{
    NTSTATUS        status;
    BOOLEAN         completeRequest = TRUE;
    WDFDEVICE       device          = WdfIoQueueGetDevice(Queue);
    PDEVICE_CONTEXT deviceContext   = NULL;
    PQUEUE_CONTEXT  queueContext    = GetQueueContext(Queue);
    UNREFERENCED_PARAMETER(OutputBufferLength);
    UNREFERENCED_PARAMETER(InputBufferLength);

    deviceContext = GetDeviceContext(device);

    switch (IoControlCode)
    {
        case IOCTL_HID_GET_DEVICE_DESCRIPTOR:
            _Analysis_assume_(deviceContext->HidDescriptor.bLength != 0);
            status =
                RequestCopyFromBuffer(Request,
                                      &deviceContext->HidDescriptor,
                                      deviceContext->HidDescriptor.bLength);
            break;

        case IOCTL_HID_GET_DEVICE_ATTRIBUTES:
            status = RequestCopyFromBuffer(
                Request,
                &queueContext->DeviceContext->HidDeviceAttributes,
                sizeof(HID_DEVICE_ATTRIBUTES));
            break;

        case IOCTL_HID_GET_REPORT_DESCRIPTOR:
            status = RequestCopyFromBuffer(
                Request,
                deviceContext->ReportDescriptor,
                deviceContext->HidDescriptor.DescriptorList[0].wReportLength);
            break;

        case IOCTL_HID_READ_REPORT:
            status = ReadReport(queueContext, Request, &completeRequest);
            break;

        case IOCTL_HID_WRITE_REPORT:
            status = WriteReport(queueContext, Request);
            break;

        case IOCTL_UMDF_HID_GET_FEATURE:
            status = GetFeature(queueContext, Request);
            break;

        case IOCTL_UMDF_HID_SET_FEATURE:
            status = SetFeature(queueContext, Request);
            break;

        case IOCTL_UMDF_HID_GET_INPUT_REPORT:
            status = GetInputReport(queueContext, Request);
            break;

        case IOCTL_UMDF_HID_SET_OUTPUT_REPORT:
            status = SetOutputReport(queueContext, Request);
            break;

        case IOCTL_HID_GET_STRING:
            status = GetString(Request);
            break;

        case IOCTL_HID_GET_INDEXED_STRING:
            status = GetIndexedString(Request);
            break;

        case IOCTL_HID_SEND_IDLE_NOTIFICATION_REQUEST:  // METHOD_NEITHER
        case IOCTL_HID_ACTIVATE_DEVICE:                 // METHOD_NEITHER
        case IOCTL_HID_DEACTIVATE_DEVICE:               // METHOD_NEITHER
        case IOCTL_GET_PHYSICAL_DESCRIPTOR:             // METHOD_OUT_DIRECT

        default:
            status = STATUS_NOT_IMPLEMENTED;
            break;
    }

    //
    // Complete the request. Information value has already been set by request
    //

    if (completeRequest)
    {
        WdfRequestComplete(Request, status);
    }
}