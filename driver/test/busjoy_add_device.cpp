#include "bus_public.h"

#include <cstddef>
#include <cstdio>
#include <cfgmgr32.h>
#include <strsafe.h>

#define USAGE \
    "Usage: Enum [-p SerialNo] Plugs in a device. SerialNo must be greater than zero.\n\
             [-u SerialNo or 0] Unplugs device(s) - specify 0 to unplug all \
                                the devices enumerated so far.\n\
             [-e SerialNo or 0] Ejects device(s) - specify 0 to eject all \
                                the devices enumerated so far.\n"

#define MAX_DEVPATH_LENGTH 256

BOOLEAN bPlugIn, bUnplug, bEject;
ULONG   SerialNo;

bool get_device_path(const LPCGUID& interface_guid,
                     wchar_t*       device_path,
                     size_t         buf_len)
{
    wchar_t*  device_interface_list        = NULL;
    ULONG     device_interface_list_length = 0;
    CONFIGRET c_ret                        = CR_SUCCESS;
    HRESULT   h_ret                        = E_FAIL;
    BOOLEAN   bRet                         = TRUE;
    PWSTR     nextInterface;

    bool b_ret = true;

    c_ret =
        CM_Get_Device_Interface_List_Size(&device_interface_list_length,
                                          (LPGUID)interface_guid,
                                          NULL,
                                          CM_GET_DEVICE_INTERFACE_LIST_PRESENT);

    if (c_ret != CR_SUCCESS)
    {
        printf("Error 0x%lx retrieving device interface list size.\n", c_ret);
        goto clean0;
    }

    if (device_interface_list_length <= 1)
    {
        bRet = FALSE;
        printf(
            "Error: No active device interfaces found.\n"
            " Is the sample driver loaded?");
        goto clean0;
    }

    device_interface_list = new wchar_t[device_interface_list_length];
    if (device_interface_list == NULL)
    {
        printf("Error allocating memory for device interface list.\n");
        goto clean0;
    }
    ZeroMemory(device_interface_list,
               device_interface_list_length * sizeof(WCHAR));

    c_ret = CM_Get_Device_Interface_ListW((LPGUID)interface_guid,
                                          NULL,
                                          device_interface_list,
                                          device_interface_list_length,
                                          CM_GET_DEVICE_INTERFACE_LIST_PRESENT);

    if (c_ret != CR_SUCCESS)
    {
        printf("Error 0x%lx retrieving device interface list.\n", c_ret);
        goto clean0;
    }

    nextInterface = device_interface_list + wcslen(device_interface_list) + 1;
    if (*nextInterface != UNICODE_NULL)
    {
        printf(
            "Warning: More than one device interface instance found. \n"
            "Selecting first matching device.\n\n");
    }
    h_ret = StringCchCopyW(device_path, buf_len, device_interface_list);
    if (FAILED(h_ret))
    {
        bRet = FALSE;
        printf("Error: StringCchCopy failed with HRESULT 0x%lx", h_ret);
        goto clean0;
    }

clean0:
    if (device_interface_list != NULL)
        free(device_interface_list);

    if (c_ret != CR_SUCCESS)
        bRet = FALSE;

    return bRet;
}

BOOLEAN
OpenBusInterface(_In_z_ LPCWSTR DevicePath)
{
    HANDLE                   file;
    ULONG                    bytes;
    BUSENUM_UNPLUG_HARDWARE  unplug;
    BUSENUM_EJECT_HARDWARE   eject;
    PBUSENUM_PLUGIN_HARDWARE hardware;
    BOOLEAN                  bSuccess = FALSE;

    printf("Opening %ws\n", DevicePath);

    file = CreateFileW(DevicePath,
                       GENERIC_READ,   // Only read access
                       0,              // FILE_SHARE_READ | FILE_SHARE_WRITE
                       NULL,           // no SECURITY_ATTRIBUTES structure
                       OPEN_EXISTING,  // No special create flags
                       0,              // No special attributes
                       NULL);          // No template file
    if (INVALID_HANDLE_VALUE == file)
    {
        printf("CreateFile failed: 0x%lx", GetLastError());
        goto End;
    }

    printf("Bus interface opened!!!\n");

    //
    // Enumerate Devices
    //

    if (bPlugIn)
    {
        printf("SerialNo. of the device to be enumerated: %lu\n", SerialNo);

        hardware = (PBUSENUM_PLUGIN_HARDWARE)malloc(
            bytes =
                (sizeof(BUSENUM_PLUGIN_HARDWARE) + BUS_HARDWARE_IDS_LENGTH));

        if (hardware)
        {
            hardware->Size     = sizeof(BUSENUM_PLUGIN_HARDWARE);
            hardware->SerialNo = SerialNo;
        }
        else
        {
            printf(
                "Couldn't allocate %lu bytes for busenum plugin hardware "
                "structure.\n",
                bytes);
            goto End;
        }

        //
        // Allocate storage for the Device ID
        //
        memcpy(
            hardware->HardwareIDs, BUS_HARDWARE_IDS, BUS_HARDWARE_IDS_LENGTH);

        if (!DeviceIoControl(file,
                             IOCTL_BUSENUM_PLUGIN_HARDWARE,
                             hardware,
                             bytes,
                             NULL,
                             0,
                             &bytes,
                             NULL))
        {
            free(hardware);
            printf("PlugIn failed:0x%lx\n", GetLastError());
            goto End;
        }

        free(hardware);
    }

    //
    // Removes a device if given the specific Id of the device. Otherwise this
    // ioctls removes all the devices that are enumerated so far.
    //

    if (bUnplug)
    {
        printf("Unplugging device(s)....\n");

        unplug.Size = bytes = sizeof(unplug);
        unplug.SerialNo     = SerialNo;
        if (!DeviceIoControl(file,
                             IOCTL_BUSENUM_UNPLUG_HARDWARE,
                             &unplug,
                             bytes,
                             NULL,
                             0,
                             &bytes,
                             NULL))
        {
            printf("Unplug failed: 0x%lx\n", GetLastError());
            goto End;
        }
    }

    //
    // Ejects a device if given the specific Id of the device. Otherwise this
    // ioctls ejects all the devices that are enumerated so far.
    //
    if (bEject)
    {
        printf("Ejecting Device(s)\n");

        eject.Size = bytes = sizeof(eject);
        eject.SerialNo     = SerialNo;
        if (!DeviceIoControl(file,
                             IOCTL_BUSENUM_EJECT_HARDWARE,
                             &eject,
                             bytes,
                             NULL,
                             0,
                             &bytes,
                             NULL))
        {
            printf("Eject failed: 0x%lx\n", GetLastError());
            goto End;
        }
    }

    printf("Success!!!\n");
    bSuccess = TRUE;

End:
    if (INVALID_HANDLE_VALUE != file)
    {
        CloseHandle(file);
    }
    return bSuccess;
}

int main(int argc, char* argv[])
{
    WCHAR devicePath[MAX_DEVPATH_LENGTH] = {0};

    bPlugIn = bUnplug = bEject = FALSE;

    if (argc < 3)
    {
        goto usage;
    }

    if (argv[1][0] == '-')
    {
        if (tolower(argv[1][1]) == 'p')
        {
            if (argv[2])
                SerialNo = (USHORT)atol(argv[2]);
            bPlugIn = TRUE;
        }
        else if (tolower(argv[1][1]) == 'u')
        {
            if (argv[2])
                SerialNo = (ULONG)atol(argv[2]);
            bUnplug = TRUE;
        }
        else if (tolower(argv[1][1]) == 'e')
        {
            if (argv[2])
                SerialNo = (ULONG)atol(argv[2]);
            bEject = TRUE;
        }
        else
        {
            goto usage;
        }
    }
    else
        goto usage;

    if (bPlugIn && 0 == SerialNo)
        goto usage;

    if (get_device_path(&GUID_DEVINTERFACE_BUSJOY,
                        devicePath,
                        sizeof(devicePath) / sizeof(devicePath[0])))
    {
        OpenBusInterface(devicePath);
    }

    return 0;
usage:
    printf(USAGE);
    exit(0);
}