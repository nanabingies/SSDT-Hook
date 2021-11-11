#include "HookFunc.h"
#include "HookEntry.h"

UNICODE_STRING drvName = RTL_CONSTANT_STRING(L"\\Device\\HookFunc");
UNICODE_STRING dosDrvName = RTL_CONSTANT_STRING(L"\\DosDevices\\HookFunc");

VOID Unload(IN PDRIVER_OBJECT pDriverObject) {
	KdPrint(("==>%s\n", __FUNCTION__));
	if (!UndoSwapFunction((ULONG)ZwOpenFile)) {
		KdPrint(("[-] Failed Driver Unload\n"));
	}
	IoDeleteDevice(pDriverObject->DeviceObject);
	IoDeleteSymbolicLink(&dosDrvName);
	KdPrint(("Successfully unloaded the driver.\n"));
	KdPrint(("<==%s\n", __FUNCTION__));
}

NTSTATUS DefaultDispatch(IN PDEVICE_OBJECT pDeviceObject, PIRP pIrp) {
	UNREFERENCED_PARAMETER(pDeviceObject);
	KdPrint(("==>%s\n", __FUNCTION__));
	pIrp->IoStatus.Status = STATUS_SUCCESS;
	pIrp->IoStatus.Information = 0;
	IofCompleteRequest(pIrp, IO_NO_INCREMENT);
	KdPrint(("<==%s\n", __FUNCTION__));
	return STATUS_SUCCESS;
}

NTSTATUS DriverEntry(IN PDRIVER_OBJECT pDriverObject, IN PUNICODE_STRING pRegistryPath) {
	UNREFERENCED_PARAMETER(pRegistryPath);

	KdPrint(("==>%s\n", __FUNCTION__));
	if (!pDriverObject) {
		KdPrint(("[-] Failed Driver Entry.\n"));
		return STATUS_FAILED_DRIVER_ENTRY;
	}

	NTSTATUS ntStatus;
	PDEVICE_OBJECT pDeviceObject;
	ntStatus = IoCreateDevice(pDriverObject, 0, &drvName, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN,
		FALSE, &pDeviceObject);
	if (!NT_SUCCESS(ntStatus)) {
		KdPrint(("[-] Failed Driver Entry.\n"));
		return STATUS_FAILED_DRIVER_ENTRY;
	}

	ntStatus = IoCreateSymbolicLink(&dosDrvName, &drvName);
	if (!NT_SUCCESS(ntStatus)) {
		KdPrint(("[-] Failed Driver Entry.\n"));
		return STATUS_FAILED_DRIVER_ENTRY;
	}

	for (ULONG i = 0; i < IRP_MJ_MAXIMUM_FUNCTION; i++) {
		pDriverObject->MajorFunction[i] = DefaultDispatch;
	}

	if (!saveSSDTEntries()) {
		KdPrint(("[-] Failed Driver Entry.\n"));
		return STATUS_FAILED_DRIVER_ENTRY;
	}

	if (!SwapFunction((ULONG)ZwOpenFile, (ULONG)ZwOpenFileHookEntry)) {
		KdPrint(("[-] Failed Driver Entry.\n"));
		return STATUS_FAILED_DRIVER_ENTRY;
	}
	KdPrint(("<==%s\n", __FUNCTION__));

	return STATUS_SUCCESS;
}