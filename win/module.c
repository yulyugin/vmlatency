/*
 * Copyright (c) 2017 Evgenii Iuliugin
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include <ntddk.h>

#include "api.h"

#define NT_DEVICE_NAME  L"\\Device\\VMLATENCY"

DRIVER_INITIALIZE DriverEntry;
DRIVER_UNLOAD VmlatencyUnloadDriver;

#ifdef ALLOC_PRAGMA
#pragma alloc_text(INIT, DriverEntry)
#endif /* ALLOC_PRAGMA */

NTSTATUS
DriverEntry(__in PDRIVER_OBJECT DriverObject,
            __in PUNICODE_STRING RegistryPath) {
        NTSTATUS ntStatus;
        UNICODE_STRING ntUnicodeString;
        UNICODE_STRING ntWin32NameString;
        PDEVICE_OBJECT deviceObject = NULL;

        RtlInitUnicodeString(&ntUnicodeString, NT_DEVICE_NAME);

        ntStatus = IoCreateDevice(DriverObject, 0, &ntUnicodeString,
                                  FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN,
                                  FALSE, &deviceObject);

        if (!NT_SUCCESS(ntStatus)) {
                vmlatency_printk("Couldn't create the device object\n");
                return ntStatus;
        }

        DriverObject->DriverUnload = VmlatencyUnloadDriver;

        return ntStatus;
}

VOID
VmlatencyUnloadDriver(__in PDRIVER_OBJECT DriverObject) {
        PDEVICE_OBJECT deviceObject = DriverObject->DeviceObject;

        if (deviceObject != NULL)
                IoDeleteDevice(deviceObject);
}
