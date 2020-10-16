#include "DriverEntry.h"

static VOID DriverUnload(_In_ PDRIVER_OBJECT DriverObject)
{
	UNREFERENCED_PARAMETER(DriverObject);
	return;
}
EXTERN_C  NTSTATUS DriverEntry(IN PDRIVER_OBJECT pDriverObj, IN PUNICODE_STRING pRegistryString)
{
	UNREFERENCED_PARAMETER(pRegistryString);
	ULONG64 _Rax = 0;

	for (size_t i = 0; i < 10; i++)
	{
		DPRINT("Test\n");
	
	}
	__asm
	{
		mov _Rax, rax
	}
	DPRINT("Rax = %x\n", _Rax);

	pDriverObj->DriverUnload = DriverUnload;

	return STATUS_UNSUCCESSFUL;
}