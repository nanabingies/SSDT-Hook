#pragma once

#include <ntifs.h>
#include <ntddk.h>

typedef struct _SYSTEM_SERVICE_TABLE {
	PULONG ServiceTable;				// array of entry points
	PULONG CounterTable;				// array of usage counters
	ULONG ServiceLimit;				// number of table entries
	PVOID ArgumentTable;				// array of byte counts
}SYSTEM_SERVICE_TABLE, *PSYSTEM_SERVICE_TABLE;

typedef struct _SERVICE_DESCRIPTOR_TABLE {
	SYSTEM_SERVICE_TABLE ntoskrnl;		// ntoskrnl.exe
	SYSTEM_SERVICE_TABLE win32k;		// win32k.sys
	SYSTEM_SERVICE_TABLE Table3;		// not used
	SYSTEM_SERVICE_TABLE Table4;		// not used
}SERVICE_DESCRIPTOR_TABLE, *PSERVICE_DESCRIPTOR_TABLE;

__declspec(dllimport) SERVICE_DESCRIPTOR_TABLE KeServiceDescriptorTable;