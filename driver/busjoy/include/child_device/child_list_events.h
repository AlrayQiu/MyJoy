#pragma once
#include <ntddk.h>
#include <wdf.h>

EVT_WDF_CHILD_LIST_CREATE_DEVICE                        BusEvtDeviceListCreatePdo;
EVT_WDF_CHILD_LIST_IDENTIFICATION_DESCRIPTION_DUPLICATE BusEvtChildListIdentificationDescriptionDuplicate;
EVT_WDF_CHILD_LIST_IDENTIFICATION_DESCRIPTION_COMPARE   BusEvtChildListIdentificationDescriptionCompare;
EVT_WDF_CHILD_LIST_IDENTIFICATION_DESCRIPTION_CLEANUP   BusEvtChildListIdentificationDescriptionCleanup;

NTSTATUS
BusPlugInDevice(_In_ WDFDEVICE Device, _In_ PWCHAR HardwareIds, _In_ size_t CchHardwareIds, _In_ ULONG SerialNo);

NTSTATUS BusUnPlugDevice(WDFDEVICE Device, ULONG SerialNo);

NTSTATUS BusEjectDevice(WDFDEVICE Device, ULONG SerialNo);
