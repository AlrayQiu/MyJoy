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

static HID_DESCRIPTOR S_MyJoyHidDescriptor = {
    0x09,    // bLength: 9
    0x21,    // bDescriptorType: 0x21 = HID_DESCRIPTOR
    0x0110,  // bcdHID: 1.1
    0x00,    // bCountry: Not specific to any country
    0x01,    // bNumDescriptors
    {
        0x22,                       // report descriptor type 0x22
        sizeof(S_ReportDescriptor)  // total length of report descriptor
    }};

typedef struct _HID_INPUT_REPORT
{
    INT8  Throttle;   // 油门 [-127,127]
    INT8  X;          // X轴  [-127,127]
    INT8  Y;          // Y轴  [-127,127]
    UINT8 HatSwitch;  // DPad 0-3 代表4个方向
    UINT8 Buttons;    // 按钮  4个位表示4个按钮
} HID_INPUT_REPORT, *PHID_INPUT_REPORT;

typedef HID_INPUT_REPORT  HID_OUT_REPORT;
typedef PHID_INPUT_REPORT PHID_OUT_REPORT;

#define ELIMINATE_UNUSED_HEADER_FILE_WARNINGS
#endif