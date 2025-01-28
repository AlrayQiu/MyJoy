#include "queue.h"
#include "request.h"
#include "processor.h"

#include "context.h"
#include "utils.h"

EVT_WDF_IO_QUEUE_IO_INTERNAL_DEVICE_CONTROL EvtIoDeviceControl;

NTSTATUS QueueCreate(IN WDFDEVICE Device, OUT WDFQUEUE *Queue)
{
    KdPrint(("Queue Create Start\r\n"));
    NTSTATUS              status;
    WDF_IO_QUEUE_CONFIG   queueConfig;
    WDF_OBJECT_ATTRIBUTES queueAttributes;
    WDFQUEUE              queue;
    PQUEUE_CONTEXT        queueContext;

    WDF_IO_QUEUE_CONFIG_INIT_DEFAULT_QUEUE(&queueConfig, WdfIoQueueDispatchParallel);

    queueConfig.EvtIoInternalDeviceControl = EvtIoDeviceControl;

    WDF_OBJECT_ATTRIBUTES_INIT_CONTEXT_TYPE(&queueAttributes, QUEUE_CONTEXT);

    status = WdfIoQueueCreate(Device, &queueConfig, &queueAttributes, &queue);
    ASSERT_STATUS_SUCCESS(status, "WdfIoQueueCreate failed");

    queueContext                = GetQueueContext(queue);
    queueContext->Queue         = queue;
    queueContext->DeviceContext = GetDeviceContext(Device);

    *Queue = queue;
    KdPrint(("Queue Create End:0x%x\r\n", status));

    return status;
}

VOID EvtIoDeviceControl(IN WDFQUEUE   Queue,
                        IN WDFREQUEST Request,
                        IN size_t     OutputBufferLength,
                        IN size_t     InputBufferLength,
                        IN ULONG      IoControlCode)
{
    KdPrint(("EvtIoDeviceControl\r\n"));
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
        KdPrint(("IOCTL_HID_GET_DEVICE_DESCRIPTOR\r\n"));
        status = RequestCopyFromBuffer(
            Request, &deviceContext->HidDescriptor, deviceContext->HidDescriptor.bLength);
        break;

    case IOCTL_HID_GET_DEVICE_ATTRIBUTES:
        KdPrint(("IOCTL_HID_GET_DEVICE_ATTRIBUTES\r\n"));
        status = RequestCopyFromBuffer(
            Request, &queueContext->DeviceContext->HidDeviceAttributes, sizeof(HID_DEVICE_ATTRIBUTES));
        break;

    case IOCTL_HID_GET_REPORT_DESCRIPTOR:
        KdPrint(("IOCTL_HID_GET_REPORT_DESCRIPTOR\r\n"));
        status = RequestCopyFromBuffer(Request,
                                       deviceContext->ReportDescriptor,
                                       deviceContext->HidDescriptor.DescriptorList[0].wReportLength);
        break;

    case IOCTL_HID_READ_REPORT:
        KdPrint(("IOCTL_HID_READ_REPORT\r\n"));
        status = ReadReport(queueContext, Request, &completeRequest);
        break;

    case IOCTL_HID_WRITE_REPORT:
        KdPrint(("IOCTL_HID_WRITE_REPORT\r\n"));
        status = WriteReport(queueContext, Request);
        break;

    case IOCTL_HID_GET_FEATURE: // METHOD_OUT_DIRECT

        status = GetFeature(queueContext, Request);
        break;

    case IOCTL_HID_SET_FEATURE: // METHOD_IN_DIRECT

        status = SetFeature(queueContext, Request);
        break;

    case IOCTL_HID_GET_INPUT_REPORT: // METHOD_OUT_DIRECT

        status = GetInputReport(queueContext, Request);
        break;

    case IOCTL_HID_SET_OUTPUT_REPORT: // METHOD_IN_DIRECT

        status = SetOutputReport(queueContext, Request);
        break;
    case IOCTL_HID_GET_STRING:
        KdPrint(("IOCTL_HID_GET_STRING\r\n"));
        status = GetString(Request);
        break;

    case IOCTL_HID_GET_INDEXED_STRING:
        KdPrint(("IOCTL_HID_GET_INDEXED_STRING\r\n"));
        status = GetIndexedString(Request);
        break;

    case IOCTL_HID_SEND_IDLE_NOTIFICATION_REQUEST: // METHOD_NEITHER
    case IOCTL_HID_ACTIVATE_DEVICE:                // METHOD_NEITHER
    case IOCTL_HID_DEACTIVATE_DEVICE:              // METHOD_NEITHER
    case IOCTL_GET_PHYSICAL_DESCRIPTOR:            // METHOD_OUT_DIRECT

    default:
        KdPrint(("STATUS_NOT_IMPLEMENTED\r\n"));
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