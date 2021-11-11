#pragma once
#include "HookFunc.h"

typedef NTSTATUS (*ZwOpenFilePtr)(
	OUT PHANDLE            FileHandle,
	IN  ACCESS_MASK        DesiredAccess,
	IN  POBJECT_ATTRIBUTES ObjectAttributes,
	OUT PIO_STATUS_BLOCK   IoStatusBlock,
	IN  ULONG              ShareAccess,
	IN  ULONG              OpenOptions
);

NTSTATUS ZwOpenFileHookEntry(OUT PHANDLE            FileHandle,
	IN  ACCESS_MASK        DesiredAccess,
	IN  POBJECT_ATTRIBUTES ObjectAttributes,
	OUT PIO_STATUS_BLOCK   IoStatusBlock,
	IN  ULONG              ShareAccess,
	IN  ULONG			   OpenOptions) {
	KdPrint(("==>%s\n", __FUNCTION__));
	ZwOpenFilePtr FilePtr = (ZwOpenFilePtr)ssdtEntries[*(PULONG)((PUCHAR)ZwOpenFile + 1)];
	KdPrint(("[+] Successfully Hooked ZwOpenFile\n"));
	KdPrint(("<==%s\n", __FUNCTION__));
	return FilePtr(FileHandle, DesiredAccess, ObjectAttributes, IoStatusBlock, ShareAccess, OpenOptions);
}