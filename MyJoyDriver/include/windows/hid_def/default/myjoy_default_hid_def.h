#pragma once

#include <Windows.h>
#include <basetsd.h>
#include <minwindef.h>
#include <wdf.h>

#include <hidport.h>
typedef UCHAR HID_REPORT_DESCRIPTOR, *PHID_REPORT_DESCRIPTOR;

#ifdef HID_DEF_INCLUDED
#error 包含多个 Hid Def文件 ,有需要请新建Hid Def 文件  \\
If you need to use the contents of different hid def files at the same time, please create a new hid def file
#else
#define HID_DEF_INCLUDED

#include "myJoy_default_hid_report_descriptor.h"

HID_DESCRIPTOR G_MyJoyHidDescriptor = {
    0x09,    // bLength: 9
    0x21,    // bDescriptorType: 0x21 = HID_DESCRIPTOR
    0x0110,  // bcdHID: 1.1
    0x00,    // bCountry: Not specific to any country
    0x01,    // bNumDescriptors
    {
        0x22,                       // report descriptor type 0x22
        sizeof(G_ReportDescriptor)  // total length of report descriptor
    }};

typedef struct _HID_INPUT_REPORT
{
    UINT8 ReportID;   // 报告 ID（如果有多个报告，可使用此字段区分）
    INT8  Throttle;   // 油门，范围从 -127 到 127
    INT8  X;          // X 轴位置，范围从 -127 到 127
    INT8  Y;          // Y 轴位置，范围从 -127 到 127
    UINT8 HatSwitch;  // 方向帽开关，值范围从 0 到 3，表示 4 个方向
    UINT8 Buttons;    // 按钮状态，使用 4 位表示 4 个按钮（每个位表示一个按钮）
} HID_INPUT_REPORT;

#endif