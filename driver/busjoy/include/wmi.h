#pragma once
#include <bus_driver.h>

extern ULONG G_BusJoyDebugLevel;

NTSTATUS BusWmiRegistration(WDFDEVICE Device);

NTSTATUS                            Bus_WmiRegistration(WDFDEVICE Device);
EVT_WDF_WMI_INSTANCE_SET_ITEM       BusEvtStdDataSetItem;
EVT_WDF_WMI_INSTANCE_SET_INSTANCE   BusEvtStdDataSetInstance;
EVT_WDF_WMI_INSTANCE_QUERY_INSTANCE BusEvtStdDataQueryInstance;