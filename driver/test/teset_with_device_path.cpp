#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <cfgmgr32.h>
#include <hidsdi.h>
#include <time.h>
#include <iostream>

bool FindMatchingDevice(LPGUID Guid, HANDLE* Handle);

bool GetStrings(HANDLE File)
{
    BOOLEAN bSuccess;
    BYTE*   buffer;
    ULONG   bufferLength;

    bufferLength = 1024;
    buffer       = new BYTE[bufferLength]();
    if (!buffer)
    {
        printf("malloc failed\n");
        return FALSE;
    }

    ZeroMemory(buffer, bufferLength);
    bSuccess = HidD_GetProductString(File,
                                     (PVOID)buffer,  // OUT PVOID  Buffer,
                                     bufferLength    // IN ULONG  BufferLength
    );

    if (!bSuccess)
    {
        printf("failed HidD_GetProductString \n");
        goto exit;
    }
    else
    {
        printf("Product string: %S\n", (PWSTR)buffer);
    }

    ZeroMemory(buffer, bufferLength);
    bSuccess = HidD_GetSerialNumberString(File,
                                          (PVOID)buffer,  // OUT PVOID  Buffer,
                                          bufferLength  // IN ULONG BufferLength
    );

    if (!bSuccess)
    {
        printf("failed HidD_GetSerialNumberString \n");
        goto exit;
    }
    else
    {
        printf("Serial number string: %S\n", (PWSTR)buffer);
    }

    ZeroMemory(buffer, bufferLength);
    bSuccess = HidD_GetManufacturerString(File,
                                          (PVOID)buffer,  // OUT PVOID  Buffer,
                                          bufferLength  // IN ULONG BufferLength
    );

    if (!bSuccess)
    {
        printf("failed HidD_GetManufacturerString \n");
        goto exit;
    }
    else
    {
        printf("Manufacturer string: %S\n", (PWSTR)buffer);
    }

exit:

    free(buffer);

    return bSuccess;
}

int main()
{
    GUID    hidguid;
    HANDLE  file     = INVALID_HANDLE_VALUE;
    BOOLEAN found    = FALSE;
    BOOLEAN bSuccess = FALSE;

    srand((unsigned)time(NULL));

    HidD_GetHidGuid(&hidguid);

    found = FindMatchingDevice(&hidguid, &file);
    if (found)
    {
        printf("...sending control request to our device\n");

        bSuccess = GetStrings(file);
        if (bSuccess == FALSE)
        {
            goto cleanup;
        }
    }
    else
    {
        printf("Failure: Could not find our HID device \n");
    }

cleanup:

    if (found && bSuccess == FALSE)
    {
        printf(
            "****** Failure: one or more commands to device failed *******\n");
    }

    if (file != INVALID_HANDLE_VALUE)
    {
        CloseHandle(file);
    }

    return (bSuccess ? 0 : 1);
}

bool FindMatchingDevice(LPGUID InterfaceGuid, HANDLE* Handle)
{
    CONFIGRET cr                        = CR_SUCCESS;
    PWSTR     deviceInterfaceList       = NULL;
    ULONG     deviceInterfaceListLength = 0;
    PWSTR     currentInterface;
    BOOLEAN   bRet      = FALSE;
    HANDLE    devHandle = INVALID_HANDLE_VALUE;

    if (NULL == Handle)
    {
        printf("Error: Invalid device handle parameter\n");
        return FALSE;
    }

    *Handle = INVALID_HANDLE_VALUE;

    cr =
        CM_Get_Device_Interface_List_Size(&deviceInterfaceListLength,
                                          InterfaceGuid,
                                          NULL,
                                          CM_GET_DEVICE_INTERFACE_LIST_PRESENT);
    if (cr != CR_SUCCESS)
    {
        printf("Error 0x%lx retrieving device interface list size.\n", cr);
        goto clean0;
    }

    if (deviceInterfaceListLength <= 1)
    {
        bRet = FALSE;
        printf(
            "Error: No active device interfaces found.\n"
            " Is the sample driver loaded?");
        goto clean0;
    }

    deviceInterfaceList =
        (PWSTR)malloc(deviceInterfaceListLength * sizeof(WCHAR));
    if (deviceInterfaceList == NULL)
    {
        printf("Error allocating memory for device interface list.\n");
        goto clean0;
    }
    ZeroMemory(deviceInterfaceList, deviceInterfaceListLength * sizeof(WCHAR));

    cr = CM_Get_Device_Interface_ListW(InterfaceGuid,
                                       NULL,
                                       deviceInterfaceList,
                                       deviceInterfaceListLength,
                                       CM_GET_DEVICE_INTERFACE_LIST_PRESENT);
    if (cr != CR_SUCCESS)
    {
        printf("Error 0x%lx retrieving device interface list.\n", cr);
        goto clean0;
    }

    //
    // Enumerate devices of this interface class
    //
    printf(
        "\n....looking for our HID device (with UP=0x "
        "and Usage=0x)\n");

    for (currentInterface = deviceInterfaceList; *currentInterface;
         currentInterface += wcslen(currentInterface) + 1)
    {
        devHandle = CreateFileW(currentInterface,
                                GENERIC_READ | GENERIC_WRITE,
                                FILE_SHARE_READ | FILE_SHARE_WRITE,
                                NULL,  // no SECURITY_ATTRIBUTES structure
                                OPEN_EXISTING,  // No special create flags
                                0,              // No special attributes
                                NULL);          // No template file

        if (INVALID_HANDLE_VALUE == devHandle)
        {
            printf("Warning: CreateFile failed: %lu\n", GetLastError());
            continue;
        }
        std::cout << currentInterface;
        if (true)
        {
            bRet    = TRUE;
            *Handle = devHandle;
        }
        else
        {
            CloseHandle(devHandle);
        }
    }

clean0:
    if (deviceInterfaceList != NULL)
    {
        free(deviceInterfaceList);
    }
    if (CR_SUCCESS != cr)
    {
        bRet = FALSE;
    }
    return bRet;
}
