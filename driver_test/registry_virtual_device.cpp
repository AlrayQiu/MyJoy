

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <devguid.h>
#include <initguid.h>
#include <setupapi.h>

#pragma comment(lib, "setupapi.lib")

int main()
{
    // 创建设备信息集合
    HDEVINFO deviceInfoSet =
        SetupDiCreateDeviceInfoList(&GUID_DEVCLASS_SYSTEM, nullptr);
    if (deviceInfoSet == INVALID_HANDLE_VALUE)
    {
        printf("Failed to create device info set\n");
        return 1;
    }

    // 创建设备信息数据
    SP_DEVINFO_DATA deviceInfoData;
    deviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
    if (!SetupDiCreateDeviceInfo(deviceInfoSet,
                                 "VirtualDevice",
                                 &GUID_DEVCLASS_SYSTEM,
                                 nullptr,
                                 nullptr,
                                 DICD_GENERATE_ID,
                                 &deviceInfoData))
    {
        printf("Failed to create device info data\n");
        SetupDiDestroyDeviceInfoList(deviceInfoSet);
        return 1;
    }

    // 设置设备安装参数
    SP_DEVINSTALL_PARAMS deviceInstallParams;
    deviceInstallParams.cbSize = sizeof(SP_DEVINSTALL_PARAMS);
    if (!SetupDiGetDeviceInstallParams(
            deviceInfoSet, &deviceInfoData, &deviceInstallParams))
    {
        printf("Failed to get device install params\n");
        SetupDiDestroyDeviceInfoList(deviceInfoSet);
        return 1;
    }

    deviceInstallParams.Flags |= DI_QUIETINSTALL;
    if (!SetupDiSetDeviceInstallParams(
            deviceInfoSet, &deviceInfoData, &deviceInstallParams))
    {
        printf("Failed to set device install params\n");
        SetupDiDestroyDeviceInfoList(deviceInfoSet);
        return 1;
    }

    // 注册设备
    if (!SetupDiCallClassInstaller(
            DIF_REGISTERDEVICE, deviceInfoSet, &deviceInfoData))
    {
        printf("Failed to register device\n");
        SetupDiDestroyDeviceInfoList(deviceInfoSet);
        return 1;
    }

    printf("Virtual device created successfully\n");
    system("pause");

    // 清理
    SetupDiDestroyDeviceInfoList(deviceInfoSet);
    return 0;
}
