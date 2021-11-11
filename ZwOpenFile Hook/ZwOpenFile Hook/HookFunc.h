#pragma once
#pragma warning(disable : 4057)
#include "Header.h"
#define SWAP_SERVICE(service) KeServiceDescriptorTable.ntoskrnl.ServiceTable[*(PULONG)((PUCHAR)service + 1)]

ULONG ssdtEntries[1024];

BOOLEAN saveSSDTEntries() {
	KdPrint(("==>%s\n", __FUNCTION__));
	for (ULONG i = 0; i < KeServiceDescriptorTable.ntoskrnl.ServiceLimit; i++) {
		ssdtEntries[i] = KeServiceDescriptorTable.ntoskrnl.ServiceTable[i];
		KdPrint(("Index : %04d\t\tAddress : %08x\n", i, ssdtEntries[i]));
	}
	KdPrint(("<==%s\n", __FUNCTION__));
	return TRUE;
}

BOOLEAN EnableWrite(ULONG value) {
	_asm {
		PUSH EAX;
		MOV EAX, value;
		MOV CR0, EAX;
		POP EAX;
		STI;
	}
	return TRUE;
}

BOOLEAN DisableWrite(PULONG value) {
	ULONG val = 0;
	_asm {
		CLI;
		PUSH EAX;
		MOV EAX, CR0;
		MOV val, EAX;
		AND EAX, 0xFFFEFFFF;
		MOV CR0, EAX;
		POP EAX;
	}
	*value = val;
	return TRUE;
}

BOOLEAN SwapFunction(ULONG value1, ULONG value2) {
	KdPrint(("==>%s\n", __FUNCTION__));
	ULONG val = 0;
	if (!DisableWrite(&val)) {
		KdPrint(("Failed to Disable Write Protection.\n"));
		return FALSE;
	}

	InterlockedExchange(&SWAP_SERVICE(value1), value2);

	if (!EnableWrite(val)) {
		KdPrint(("Failed to Enable Write Protection.\n"));
		return FALSE;
	}
	KdPrint(("<==%s\n", __FUNCTION__));
	return TRUE;
}

BOOLEAN UndoSwapFunction(ULONG value1) {
	KdPrint(("==>%s\n", __FUNCTION__));
	ULONG val = 0;
	if (!DisableWrite(&val)) {
		KdPrint(("Failed to Disable Write Protection.\n"));
		return FALSE;
	}

	InterlockedExchange(&SWAP_SERVICE(value1), ssdtEntries[(ULONG)value1]);

	if (!EnableWrite(val)) {
		KdPrint(("Failed to Enable Write Protection.\n"));
		return FALSE;
	}

	KdPrint(("<==%s\n", __FUNCTION__));
	return TRUE;
}