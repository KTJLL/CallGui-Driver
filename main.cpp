#pragma warning(push, 0)
#include <ntifs.h>
#include <ntddk.h>
#include <intrin.h>
#pragma warning(pop)

#include "define.h"

//#define OB_PREOP_SUCCESS 0
//#define OB_PREOP_DISALLOW 1

enum {
	OPEN_PROCESS_IOCTL_BASE = 0x800, 
	TERMINATE_PROCESS_IOCTL_BASE, 
	PROTECT_PROCESS_IOCTL_BASE, 
	QUERY_PROCESS_IOCTL_BASE, 
	QUERY_THREADS_IOCTL_BASE, 
	TERMINATE_THREAD_IOCTL_BASE, 
	QUERY_HANDLE_IOCTL_BASE, 
	CLOSE_HANDLE_IOCTL_BASE, 
	SUSPEND_PROCESS_IOCTL_BASE, 
	RESUME_PROCESS_IOCTL_BASE, 
	QUERY_SSDT_DATA_IOCTL_BASE, 
	GET_NTOSKRNL_BASE_IOCTL_BASE, 
	GET_KISERVICETABLE_BASE_IOCTL_BASE, 
	QUERY_SSSDT_DATA_IOCTL_BASE, 
	QUERY_OBJECT_TYPE_IOCTL_BASE, 
	QUERY_OBJECT_TYPE_CALLBACK_IOCTL_BASE
};

/*
#define OPEN_PROCESS_IOCTL_BASE 0x800
#define TERMINATE_PROCESS_IOCTL_BASE 0x801
#define PROTECT_PROCESS_IOCTL_BASE 0x802
#define QUERY_PROCESS_IOCTL_BASE 0x803
#define QUERY_THREADS_IOCTL_BASE 0x804
#define TERMINATE_THREAD_IOCTL_BASE 0x805
#define QUERY_HANDLE_IOCTL_BASE 0x806
#define CLOSE_HANDLE_IOCTL_BASE 0x807
#define SUSPEND_PROCESS_IOCTL_BASE 0x808
#define RESUME_PROCESS_IOCTL_BASE 0x809
#define QUERY_SSDT_DATA_IOCTL_BASE 0x80A
#define GET_NTOSKRNL_BASE_IOCTL_BASE 0x80B
#define GET_KISERVICETABLE_BASE_IOCTL_BASE 0x80C
*/
#define CTL_CODE_OPEN_PROCESS CTL_CODE(FILE_DEVICE_UNKNOWN, OPEN_PROCESS_IOCTL_BASE, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define CTL_CODE_TERMINATE_PROCESS CTL_CODE(FILE_DEVICE_UNKNOWN, TERMINATE_PROCESS_IOCTL_BASE, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define CTL_CODE_PROTECT_PROCESS CTL_CODE(FILE_DEVICE_UNKNOWN, PROTECT_PROCESS_IOCTL_BASE, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define CTL_CODE_QUERY_PROCESS CTL_CODE(FILE_DEVICE_UNKNOWN, QUERY_PROCESS_IOCTL_BASE, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define CTL_CODE_QUERY_THREADS CTL_CODE(FILE_DEVICE_UNKNOWN, QUERY_THREADS_IOCTL_BASE, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define CTL_CODE_TERMINATE_THREAD CTL_CODE(FILE_DEVICE_UNKNOWN, TERMINATE_THREAD_IOCTL_BASE, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define CTL_CODE_QUERY_HANDLE CTL_CODE(FILE_DEVICE_UNKNOWN, QUERY_HANDLE_IOCTL_BASE, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define CTL_CODE_CLOSE_HANDLE CTL_CODE(FILE_DEVICE_UNKNOWN, CLOSE_HANDLE_IOCTL_BASE, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define CTL_CODE_SUSPEND_PROCESS CTL_CODE(FILE_DEVICE_UNKNOWN, SUSPEND_PROCESS_IOCTL_BASE, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define CTL_CODE_RESUME_PROCESS CTL_CODE(FILE_DEVICE_UNKNOWN, RESUME_PROCESS_IOCTL_BASE, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define CTL_CODE_QUERY_SSDT_DATA CTL_CODE(FILE_DEVICE_UNKNOWN, QUERY_SSDT_DATA_IOCTL_BASE, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define CTL_CODE_GET_NTOSKRNL_BASE CTL_CODE(FILE_DEVICE_UNKNOWN, GET_NTOSKRNL_BASE_IOCTL_BASE, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define CTL_CODE_GET_KISERVICETABLE_BASE CTL_CODE(FILE_DEVICE_UNKNOWN, GET_KISERVICETABLE_BASE_IOCTL_BASE, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define CTL_CODE_QUERY_SSSDT_DATA CTL_CODE(FILE_DEVICE_UNKNOWN, QUERY_SSSDT_DATA_IOCTL_BASE, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define CTL_CODE_QUERY_OBJECT_TYPE_DATA CTL_CODE(FILE_DEVICE_UNKNOWN, QUERY_OBJECT_TYPE_IOCTL_BASE, METHOD_BUFFERED, FILE_ANY_ACCESS)
#define CTL_CODE_QUERY_OBJECT_TYPE_CALLBACK CTL_CODE(FILE_DEVICE_UNKNOWN, QUERY_OBJECT_TYPE_CALLBACK_IOCTL_BASE, METHOD_BUFFERED, FILE_ANY_ACCESS)

//NTSYSAPI PVOID PsGetThreadTeb(PETHREAD Thread);
//NTSYSAPI KPRIORITY PsGetThreadBasePriority(PETHREAD Thread);
//NTSYSAPI UCHAR PsGetThreadState(PETHREAD Thread);
//NTSYSAPI PVOID PsGetThreadStartAddress(PETHREAD Thread);
//NTSYSAPI NTSTATUS PsGetNextProcessThread(
//    _In_ PEPROCESS Process,
//    _In_opt_ PETHREAD Thread,
//    _Out_ PETHREAD* OutThread
//);
//typedef KPRIORITY (NTSYSAPI *_PsGetThreadBasePriority)

//_Function_class_(DRIVER_UNLOAD)
VOID DriverUnload(PDRIVER_OBJECT DriverObject) {
	UNICODE_STRING symbolLinkName;
	RtlInitUnicodeString(&symbolLinkName, L"\\??\\KTJLL_SuperRun");
	IoDeleteSymbolicLink(&symbolLinkName);
	PDEVICE_OBJECT DeviceObject = DriverObject->DeviceObject;
	if (DeviceObject){
		PDEVICE_EXTENSION pDevExt = (PDEVICE_EXTENSION)DeviceObject->DeviceExtension;
		if (pDevExt->OpenProcessCallbackHandle){
			ObUnRegisterCallbacks(pDevExt->OpenProcessCallbackHandle);
			if (pDevExt->OpenProcessCallbackContext){
				ExFreePoolWithTag(pDevExt->OpenProcessCallbackContext, 'PsCR');
			}
		}
		IoDeleteDevice(DeviceObject);
	}

	//KeBugCheckEx(IRQL_NOT_LESS_OR_EQUAL, 0, 0, 0, 0);
	//DbgPrint("DriverUnload!\n");
} 

NTSTATUS OpenProcess(HANDLE pid, PHANDLE ProcessHandle){
	NTSTATUS status;
	
	OBJECT_ATTRIBUTES oa;
	CLIENT_ID cid = {(HANDLE)(ULONG_PTR)pid, NULL};
	InitializeObjectAttributes(&oa, NULL, 0, NULL, NULL);
	
	status = ZwOpenProcess(ProcessHandle, PROCESS_ALL_ACCESS, &oa, &cid);
	if (!NT_SUCCESS(status)){
		return status;
	}
	
	return STATUS_SUCCESS;
}

NTSTATUS DispatchCloseHandle(PVOID inputBuffer, ULONG inputLength){
	NTSTATUS status;
	
	DbgPrint("CTL CloseHandle.\n");
	
	if (!inputBuffer){
		return STATUS_INVALID_USER_BUFFER;
	}
	
	if (inputLength < sizeof(ULONG64)*2){
		return STATUS_BUFFER_TOO_SMALL;
	}
	
	HANDLE hProcess;
	
	/*OBJECT_ATTRIBUTES oa;
	CLIENT_ID cid = {*(PHANDLE)(ULONG64)inputBuffer, NULL};
	InitializeObjectAttributes(&oa, NULL, 0, NULL, NULL);
	
	status = ZwOpenProcess(&hProcess, PROCESS_ALL_ACCESS, &oa, &cid);
	if (!NT_SUCCESS(status)){
		return status;
	}*/
	status = OpenProcess(*(PHANDLE)inputBuffer, &hProcess);
	if (!NT_SUCCESS(status)){
		return status;
	}
	
	HANDLE hDummy, hRemote = *(PHANDLE)((ULONG64)inputBuffer + sizeof(ULONG64));
	status = ZwDuplicateObject(hProcess, hRemote, ZwCurrentProcess(), &hDummy, 0, 0, DUPLICATE_CLOSE_SOURCE);
	if (!NT_SUCCESS(status)){
		return status;
	}
	
	ZwClose(hDummy);
	
	return STATUS_SUCCESS;
}

void EnumFirstHandleTable(ULONG64 HandleTable, PQUERY_PROCESS_INFORMATION Qpinf){
	/*for(int j=0;j<=1;j++){
		if (!HandleTable || !MmIsAddressValid((PVOID)HandleTable)) return;
		for(int i=0;i<0x100;i++){
			if (*(ULONG64*)HandleTable && MmIsAddressValid((PVOID)*(ULONG64*)HandleTable) && !*(ULONG32*)(HandleTable+0x8)){
				ULONG64 Object = *(ULONG64*)HandleTable & 0xFFFFFFFFFFFFFFF0; //取高位
				ULONG64 ObjectHeader = Object - 0x30;
				UCHAR TypeIndex;
				if(MmIsAddressValid((PVOID)(ObjectHeader+0x18))){
					TypeIndex = *(PUCHAR)(ObjectHeader+0x18);
					ULONG64 ObjectType = ((ULONG64*)ObTypeIndexTable)[TypeIndex];
					DbgPrint("Object: 0x%p Type: 0x%p Entry: 0x%p\n", Object, ObjectType, HandleTable);					
				}
			}
			HandleTable += 0x10;
		}
	}*/
	if (!HandleTable || !MmIsAddressValid((PVOID)HandleTable)) return;
	for(int i=0;i<0x100;i++){
		if (*(ULONG64*)HandleTable && MmIsAddressValid((PVOID)*(ULONG64*)HandleTable) && !*(ULONG32*)(HandleTable+0x8)){
			ULONG64 Object = *(ULONG64*)HandleTable & 0xFFFFFFFFFFFFFFF0; //取高位
			ULONG64 ObjectHeader = Object - 0x30;
			UCHAR TypeIndex;
			if(MmIsAddressValid((PVOID)(ObjectHeader+0x18))){
				TypeIndex = *(PUCHAR)(ObjectHeader+0x18);
				ULONG64 ObjectType = ((ULONG64*)ObTypeIndexTable)[TypeIndex];
				if(MmIsAddressValid((PVOID)ObjectType)){
					UNICODE_STRING Type = *(PUNICODE_STRING)(ObjectType+0x10);
					DbgPrint("Object: 0x%p Type: 0x%p Entry: 0x%p Name: %wZ\n", Object, ObjectType, HandleTable, Type);		
				}
							
			}		
		}	
		HandleTable += 0x10;
	}
}

void EnumSecondHandleTable(ULONG64 HandleTable, PQUERY_PROCESS_INFORMATION Qpinf){
	while (*(ULONG64*)HandleTable && MmIsAddressValid((PVOID)*(ULONG64*)HandleTable)){
		EnumFirstHandleTable(*(ULONG64*)HandleTable, Qpinf);
		HandleTable += sizeof(ULONG64);
	}
}

void EnumThirdHandleTable(ULONG64 HandleTable, PQUERY_PROCESS_INFORMATION Qpinf){
	while (*(ULONG64*)HandleTable && MmIsAddressValid((PVOID)*(ULONG64*)HandleTable)){
		EnumSecondHandleTable(*(ULONG64*)HandleTable, Qpinf);
		HandleTable += sizeof(ULONG64);
	}
}

void EnumPspCidTable(){
	PVOID CidHandleTable = *(PVOID*)PspCidTable;
	ULONG64 TableCode = *(ULONG64*)CidHandleTable;
	unsigned char TableFlag = TableCode & 0x3; //取低3位
	TableCode &= 0xFFFFFFFFFFFFFFFC; //取高位;
	
	switch (TableFlag) {
		case 0:
			EnumFirstHandleTable(TableCode, NULL);
			break;
		case 1:
			EnumSecondHandleTable(TableCode, NULL);
			break;
		case 2:
			EnumThirdHandleTable(TableCode, NULL);
			break;
		default:
			return;
	}
}

NTSTATUS DispatchQueryObjectCallbacks(PVOID outputBuffer, ULONG outputLength, PIRP Irp){	
	if(!outputBuffer){
		return STATUS_INVALID_USER_BUFFER;
	}
	
	if (!outputLength){
		return STATUS_BUFFER_TOO_SMALL;
	}
	
	LIST_ENTRY ProcessCallBackList = *(PLIST_ENTRY)((ULONG64)*PsProcessType + 0xc0);
	POB_CALLBACK pProcessObCallBack = (POB_CALLBACK)ProcessCallBackList.Flink;
	ULONG CallbackCount = 0, index = 0;
	do{
		if (!pProcessObCallBack || !MmIsAddressValid(pProcessObCallBack)){
			pProcessObCallBack = (POB_CALLBACK)pProcessObCallBack->ListEntry.Flink;
			continue;
		}
		if (!pProcessObCallBack->ObHandle){
			pProcessObCallBack = (POB_CALLBACK)pProcessObCallBack->ListEntry.Flink;
			continue;
		}
		CallbackCount ++;
	}while(pProcessObCallBack != (POB_CALLBACK)ProcessCallBackList.Flink);
	SIZE_T Size = sizeof(QUERY_OBJECT_TYPE_CALLBACK_INFORMATION) - sizeof(CALLBACK_INFORMATION) + CallbackCount * sizeof(CALLBACK_INFORMATION);
	PQUERY_OBJECT_TYPE_CALLBACK_INFORMATION pOinf = (PQUERY_OBJECT_TYPE_CALLBACK_INFORMATION)ExAllocatePoolWithTag(PagedPool, Size, 'OCQU');
	if(!pOinf){
		return STATUS_INSUFFICIENT_RESOURCES;
	}
	
	if (outputLength < Size){
		return STATUS_BUFFER_TOO_SMALL;
	}
	pOinf->numberOfCallbacks = CallbackCount;
	
	do{
		if (!pProcessObCallBack || !MmIsAddressValid(pProcessObCallBack)){
			break;
		}
		if (!pProcessObCallBack->ObHandle){
			pProcessObCallBack = (POB_CALLBACK)pProcessObCallBack->ListEntry.Flink;
			continue;
		}
		DbgPrint("Process 0x%llX: Pre 0x%llX Post 0x%llX", pProcessObCallBack, pProcessObCallBack->PreCall, pProcessObCallBack->PostCall);
		pOinf->infos[index].bFromPsProcessType = TRUE;
		pOinf->infos[index].PostCallback = (ULONG64)pProcessObCallBack->PostCall;
		pOinf->infos[index].PreCallback = (ULONG64)pProcessObCallBack->PreCall;
		pProcessObCallBack = (POB_CALLBACK)pProcessObCallBack->ListEntry.Flink;
		index ++;
	} while(pProcessObCallBack != (POB_CALLBACK)ProcessCallBackList.Flink && index < CallbackCount);
	
	Irp->IoStatus.Information = Size;
	memcpy(outputBuffer, pOinf, Size);
	
	ExFreePoolWithTag(pOinf, 'OCQU');
	
	return STATUS_SUCCESS;
}

NTSTATUS DispatchQueryObjectTableData(PVOID outputBuffer, ULONG outputLength, PIRP Irp){
	NTSTATUS status;
	
	DbgPrint("CTL QueryObjectTableData!\n");
	
	if (!outputBuffer){
		return STATUS_INVALID_USER_BUFFER;
	}
	
	if (outputLength == 0){
		return STATUS_BUFFER_TOO_SMALL;
	}
	
	PVOID ObpTypeObjectType = (PVOID)((ULONG64*)ObTypeIndexTable)[2];
	if (!ObpTypeObjectType || !MmIsAddressValid(ObpTypeObjectType)){
		return STATUS_UNSUCCESSFUL;
	}
	ULONG CountOfTypes = *(PULONG)((ULONG64)ObpTypeObjectType + 0x2c);
	
	SIZE_T size = sizeof(QUERY_OBJECT_TYPE_INFORMATION) - sizeof(WCHAR) * 24 + sizeof(WCHAR) * CountOfTypes * 24;
	DbgPrint("Size: %d \n", size);
	
	if (outputLength < size){
		return STATUS_BUFFER_TOO_SMALL;
	}
	
	PQUERY_OBJECT_TYPE_INFORMATION poinf = (PQUERY_OBJECT_TYPE_INFORMATION)ExAllocatePoolWithTag(PagedPool, size, 'ObQU');
	if (!poinf) {
		return STATUS_INSUFFICIENT_RESOURCES;
	}
	memset(poinf, 0, size);
	
	poinf->numberOfObjectTypes = CountOfTypes;
	
	ULONG WritingCount = 0;
	for(int i=2;i<CountOfTypes+2 && i<0x100;i++){
		ULONG64 pObjectType = ((ULONG64*)ObTypeIndexTable)[i];
		if(!pObjectType || !MmIsAddressValid((PVOID)pObjectType)){
			continue;
		}
		PVOID ObjectType = *(PVOID*)pObjectType;
		if(!pObjectType || !MmIsAddressValid((PVOID)pObjectType)){
			continue;
		}
		UNICODE_STRING ObjectName = *(PUNICODE_STRING)((ULONG64)ObjectType + 0x10);
		if((ObjectName.Length >> 1) < 24 && ObjectName.Buffer){
			memcpy(&poinf->names[WritingCount], ObjectName.Buffer, ObjectName.Length * sizeof(WCHAR));
		} else {
			WritingCount ++;
			DbgPrint("Buffer invalid: 0x%llX %d", ObjectName.Buffer, ObjectName.Length);
			continue;
		}
		DbgPrint("Name: %wZ\n", &ObjectName);
		WritingCount ++;
	}
	
	memcpy(outputBuffer, poinf, size);
	Irp->IoStatus.Information = size;
	
	ExFreePoolWithTag(poinf, 'ObQU');
	return STATUS_SUCCESS;
}

NTSTATUS DispatchQuerySSSDTData(PVOID inputBuffer, ULONG inputLength, PVOID outputBuffer, ULONG outputLength, PIRP Irp){
	NTSTATUS status;
	
	if (!outputBuffer || !inputBuffer){
		return STATUS_INVALID_USER_BUFFER;
	}
	
	if (inputLength < sizeof(ULONG64)){
		return STATUS_BUFFER_TOO_SMALL;
	}
	
	ULONG numberOfFunctions = *(ULONG64*)inputBuffer;
	SIZE_T size = sizeof(QUERY_SSSDT_INFORMATION) + (numberOfFunctions-1)*sizeof(ULONG64);
	
	if (outputLength < size){
		return STATUS_BUFFER_TOO_SMALL;
	}
	
	PQUERY_SSSDT_INFORMATION psinf = (PQUERY_SSSDT_INFORMATION)ExAllocatePoolWithTag(PagedPool, size, 'SSQU');
	if (!psinf){
		return STATUS_INSUFFICIENT_RESOURCES;
	}
	
	psinf->Win32kBase = (ULONG64)Win32kBase;
	psinf->w32pServiceTableAddr = (ULONG64)AddrSSSDT[1].ServiceTableBase;
	for(unsigned int i=0;i<numberOfFunctions;i++){
		GetSSDTandSSSDTFuncByIndex(FALSE, i, (PVOID*)&psinf->Addrs[i]);
	}
	
	memcpy(inputBuffer, psinf, size);
	Irp->IoStatus.Information = size;
	
	ExFreePoolWithTag(psinf, 'SSQU');
	
	return STATUS_SUCCESS;
}

NTSTATUS GetNtOsKrnlAndWin32kBase(PDRIVER_OBJECT DriverObject){
	/*NTSTATUS status;
	
	ULONG need = 0;
	status = ZwQuerySystemInformation(SystemModuleInformation, NULL, 0, &need);
	if (status != STATUS_INFO_LENGTH_MISMATCH){
		return status;
	}
	
	PRTL_PROCESS_MODULES info = (PRTL_PROCESS_MODULES)ExAllocatePoolWithTag(PagedPool, need, 'GKnB');
	if (!info){
		return STATUS_INSUFFICIENT_RESOURCES;
	}
	
	status = ZwQuerySystemInformation(SystemModuleInformation, info, need, &need);
	if (!NT_SUCCESS(status)){
		ExFreePoolWithTag(info, 'GKnB');
		return status;
	}
	
	if (info->NumberOfModules == 0){
		ExFreePoolWithTag(info, 'GKnB');
		return STATUS_UNSUCCESSFUL;
	}
	
	NtOsKrnlBase = info->Modules[0].ImageBase;
	ExFreePoolWithTag(info, 'GKnB');*/
	PLDR_DATA_TABLE_ENTRY_KERNEL ldte = (PLDR_DATA_TABLE_ENTRY_KERNEL)DriverObject->DriverSection;
	if(!ldte){
		return STATUS_UNSUCCESSFUL;
	}
	
	LIST_ENTRY *head, *cur;
	head = &ldte->InLoadOrderLinks;
	cur = head->Flink;
	
	UNICODE_STRING NtOsKrnlName = RTL_CONSTANT_STRING(L"ntoskrnl.exe");
	UNICODE_STRING win32kName = RTL_CONSTANT_STRING(L"win32k.sys");
	
	while(1){
		ldte = CONTAINING_RECORD(cur, LDR_DATA_TABLE_ENTRY_KERNEL, InLoadOrderLinks);
		if (!ldte || !ldte->BaseDllName.Buffer || ldte->BaseDllName.Length == 0){
			cur = cur->Flink;
			continue;
		}
		if(RtlCompareUnicodeString(&NtOsKrnlName, &ldte->BaseDllName, true) == 0){
			NtOsKrnlBase = ldte->DllBase;
		}
		if(RtlCompareUnicodeString(&win32kName, &ldte->BaseDllName, true) == 0){
			Win32kBase = ldte->DllBase;
		}
		if(cur == head){
			break;
		}
		cur = cur->Flink;
	}
	
	return STATUS_SUCCESS;
}
/*
NTSTATUS DispatchGetNtOsKrnlBase(PVOID outputBuffer, ULONG outputLength, PIRP Irp){
	NTSTATUS status;
	
	if (!outputBuffer){
		return STATUS_INVALID_USER_BUFFER;
	}
	
	if (outputLength < sizeof(ULONG64)){
		return STATUS_BUFFER_TOO_SMALL;
	}
	
	memcpy(outputBuffer, (PVOID)&NtOsKrnlBase, sizeof(ULONG64));
	Irp->IoStatus.Information = sizeof(ULONG64);
	
	return STATUS_SUCCESS;
}*/

NTSTATUS DispatchQuerySSDTData(PVOID inputBuffer, ULONG inputLength, PVOID outputBuffer, ULONG outputLength, PIRP Irp){
	NTSTATUS status;
	
	if (!outputBuffer || !inputBuffer){
		return STATUS_INVALID_USER_BUFFER;
	}
	
	if (inputLength < sizeof(ULONG64)){
		return STATUS_BUFFER_TOO_SMALL;
	}
	
	ULONG numberOfFunctions = *(ULONG64*)inputBuffer;
	SIZE_T size = sizeof(QUERY_SSDT_INFORMATION) + (numberOfFunctions-1)*sizeof(ULONG64);
	
	if (outputLength < size){
		return STATUS_BUFFER_TOO_SMALL;
	}
	
	PQUERY_SSDT_INFORMATION psinf = (PQUERY_SSDT_INFORMATION)ExAllocatePoolWithTag(PagedPool, size, 'SdQU');
	if (!psinf){
		return STATUS_INSUFFICIENT_RESOURCES;
	}
	
	for(unsigned int i=0;i<numberOfFunctions;i++){
		GetSSDTandSSSDTFuncByIndex(TRUE, i, (PVOID*)&psinf->Addrs[i]);
	}
	
	//*(ULONG64*)((ULONG64)psinf+numberOfFunctions*sizeof(ULONG64)) = (ULONG64)*(ULONG64*)AddrSSDT;
	//*(ULONG64*)((ULONG64)psinf+(numberOfFunctions+1)*sizeof(ULONG64)) = (ULONG64)NtOsKrnlBase;
	psinf->KernelBase = (ULONG64)NtOsKrnlBase;
	psinf->KiServiceTableAddr = (ULONG64)AddrSSDT->ServiceTableBase;
	//DbgPrint("0x%p 0x%p", psinf->KiServiceTableAddr, psinf->KernelBase);
	
	memcpy(inputBuffer, psinf, size);
	Irp->IoStatus.Information = size;
	
	ExFreePoolWithTag(psinf, 'SdQU');
	
	return STATUS_SUCCESS;
}

/*
NTSTATUS DispatchResumeProcess(PVOID inputBuffer, ULONG inputLength){
	NTSTATUS status;
	
	if (!inputBuffer){
		return STATUS_INVALID_USER_BUFFER;
	}
	
	if (inputLength < sizeof(ULONG64)){
		return STATUS_BUFFER_TOO_SMALL;
	}
	
	HANDLE dwPID = *((PHANDLE)inputBuffer);
	HANDLE hProcess;
	
	OpenProcess(dwPID, &hProcess);
	
	status = NtResumeProcess(hProcess);
	if (!NT_SUCCESS(status)){
		ZwClose(hProcess);
		return status;
	}
	
	ZwClose(hProcess);
	return STATUS_SUCCESS;
}

NTSTATUS DispatchSuspendProcess(PVOID inputBuffer, ULONG inputLength){
	NTSTATUS status;
	
	if (!inputBuffer){
		return STATUS_INVALID_USER_BUFFER;
	}
	
	if (inputLength < sizeof(ULONG64)){
		return STATUS_BUFFER_TOO_SMALL;
	}
	
	HANDLE dwPID = *((PHANDLE)inputBuffer);
	HANDLE hProcess;
	
	OpenProcess(dwPID, &hProcess);
	
	status = NtSuspendProcess(hProcess);
	if (!NT_SUCCESS(status)){
		ZwClose(hProcess);
		return status;
	}
	
	ZwClose(hProcess);
	return STATUS_SUCCESS;
}
*/

BOOLEAN EnumHandle(PVOID HandleTableEntry, HANDLE handle, PVOID pQueryHandleInformation){
	DbgPrint("Handle: 0x%08X\n", handle);
	
	if (!pQueryHandleInformation){
		return TRUE;
	}
	
	PQUERY_HANDLE_INFORMATION pQinf = (PQUERY_HANDLE_INFORMATION)pQueryHandleInformation;
	SIZE_T curSize = sizeof(QUERY_HANDLE_INFORMATION) - sizeof(QUERY_HANDLE_INFORMATION) + pQinf->numberOfHandles * sizeof(HANDLE_INFORMATION);
	
	if (curSize >= pQinf->sizeOfInfo){
		return TRUE;
	}
	
	pQinf->infos[pQinf->numberOfHandles].HandleValue = (USHORT)handle;
	if (HandleTableEntry && MmIsAddressValid(HandleTableEntry)){
		ULONG64 ObjectHeader = *(PULONG64)HandleTableEntry & 0xFFFFFFFFFFFFFFF0;
		ULONG64 ObjectAddr = ObjectHeader + 0x30;
		pQinf->infos[pQinf->numberOfHandles].ObjectAddr = ObjectAddr;
		if (ObjectHeader && MmIsAddressValid((PVOID)ObjectHeader)){
			UCHAR TypeIndex = *(PUCHAR)(ObjectHeader + 0x18);
			pQinf->infos[pQinf->numberOfHandles].objectType= TypeIndex;
		}
		pQinf->infos[pQinf->numberOfHandles].GrantedAccess = *(PULONG32)(ObjectAddr + 0x08);
	}
	pQinf->numberOfHandles ++;
	return FALSE;
}

NTSTATUS DispatchQueryProcessHandle(PVOID inputBuffer, ULONG inputLength, PVOID outputBuffer, ULONG outputLength, PIRP Irp){
	NTSTATUS status;
	
	DbgPrint("CTL QueryHandle\n");
	
	if (!inputBuffer || !outputBuffer){
		return STATUS_INVALID_USER_BUFFER;
	}
	
	if (inputLength < sizeof(ULONG64) || !outputLength){
		return STATUS_BUFFER_TOO_SMALL;
	}
	
	//ULONG retLength = 0;
	/*status = ZwQuerySystemInformation(SystemHandleInformation, NULL, 0, &retLength);
	if (status != STATUS_INFO_LENGTH_MISMATCH){
		DbgPrint("ZwQuerySystemInformation failed\n");
		return status;
	}*/
	/*
	//bufferSize += 8192;
	ULONG retLength = 1048576;
	//DbgPrint("NeedMemory %ld", retLength);
	
	PSYSTEM_HANDLE_INFORMATION info = (PSYSTEM_HANDLE_INFORMATION)ExAllocatePoolWithTag(PagedPool, retLength, 'HdQr');
	if (!info){
		return STATUS_INSUFFICIENT_RESOURCES;
	}
	
	status = ZwQuerySystemInformation(SystemHandleInformation, info, retLength, &retLength);
	if (!NT_SUCCESS(status)){
		//DbgPrint("aa memory: %ld", retLength);
		return status;
	}
	
	ULONG32 handleCount = info->NumberOfHandles;
	ULONG64 size = sizeof(QUERY_HANDLE_INFORMATION) + handleCount*sizeof(HANDLE_INFORMATION);
	*/
	/*if (outputLength < size){
		ExFreePoolWithTag(info, 'HdQr');
		return STATUS_BUFFER_TOO_SMALL;
	}*/
	/*
	PQUERY_HANDLE_INFORMATION phinf = (PQUERY_HANDLE_INFORMATION)ExAllocatePoolWithTag(PagedPool, size, 'HdQU');
	if (!phinf){
		ExFreePoolWithTag(info, 'HdQr');
		return STATUS_INSUFFICIENT_RESOURCES;
	}
	
	ULONG64 index = 0;
	for (ULONG i=0;i<handleCount;i++){
		if (info->Handles[i].UniqueProcessId == (ULONG64)(ULONG_PTR)*(PULONG)inputBuffer){
			phinf->infos[index].objectType = info->Handles[i].ObjectTypeIndex;
			phinf->infos[index].HandleValue = info->Handles[i].HandleValue;
			phinf->infos[index].ObjectAddr = (ULONG64)info->Handles[i].Object;
			phinf->infos[index].GrantedAccess = info->Handles[i].GrantedAccess;
			index++;
		}
	}
	
	SIZE_T sizeOfUserBuffer = sizeof(QUERY_HANDLE_INFORMATION) + index*sizeof(HANDLE_INFORMATION);
	if (outputLength < sizeOfUserBuffer){
		ExFreePoolWithTag(info, 'HdQr');
		ExFreePoolWithTag(phinf, 'HdQU');
		return STATUS_BUFFER_TOO_SMALL;
	}
	
	phinf->sizeOfInfo = sizeOfUserBuffer;
	phinf->numberOfHandles = index + 1;
	memcpy(outputBuffer, phinf, sizeOfUserBuffer);
	Irp->IoStatus.Information = sizeOfUserBuffer;
	
	ExFreePoolWithTag(info, 'HdQr');
	ExFreePoolWithTag(phinf, 'HdQU');
	*/
	
	PEPROCESS TargetProcess;
	status = PsLookupProcessByProcessId(*(PHANDLE)inputBuffer, &TargetProcess);
	if (!NT_SUCCESS(status)){
		//DbgPrint("Get EPROCESS failed!\n");
		return status;
	}
	
	PVOID pHandleTable = *(PVOID*)((ULONG64)TargetProcess + 0x200);
	if (!pHandleTable || !MmIsAddressValid(pHandleTable)){
		return STATUS_UNSUCCESSFUL;
	}
	
	LONG HandleCount = *(PULONG)((ULONG64)pHandleTable + 0x58);
	SIZE_T size = sizeof(QUERY_HANDLE_INFORMATION) + (HandleCount - 1) * sizeof(HANDLE_INFORMATION);
	
	//DbgPrint("%d\n", HandleCount);
	PQUERY_HANDLE_INFORMATION phinf = (PQUERY_HANDLE_INFORMATION)ExAllocatePoolWithTag(PagedPool, size, 'HdQU');
	if (!phinf){
		ExFreePoolWithTag(phinf, 'HdQU');
		return STATUS_INSUFFICIENT_RESOURCES;
	}
	
	phinf->numberOfHandles = 0;
	phinf->sizeOfInfo = size;
	
	HANDLE unused;
	ExEnumHandleTable(pHandleTable, (EX_ENUMERATE_HANDLE_ROUTINE)EnumHandle, phinf, &unused);
	
	//DbgPrint("EnumSuccess!\n");
	
	if (phinf->sizeOfInfo < sizeof(QUERY_HANDLE_INFORMATION) - sizeof(HANDLE_INFORMATION) + phinf->numberOfHandles * sizeof(HANDLE_INFORMATION)){
		ExFreePoolWithTag(phinf, 'HdQU');
		return STATUS_UNSUCCESSFUL;
	}
	
	memcpy(outputBuffer, phinf, size);
	Irp->IoStatus.Information = size;
	
	ExFreePoolWithTag(phinf, 'HdQU');
	return STATUS_SUCCESS;
}

NTSTATUS DispatchTerminateThread(PVOID inputBuffer, ULONG inputLength){
	NTSTATUS status;
	
	//DbgPrint("CTL TerminateThread\n");
	
	if (!inputBuffer){
		return STATUS_INVALID_USER_BUFFER;
	}
	
	if (inputLength < sizeof(HANDLE)){
		return STATUS_BUFFER_TOO_SMALL;
	}
	
	PETHREAD EThread;
	HANDLE threadID = *(PHANDLE)inputBuffer;
	//DbgPrint("tid: %p", (ULONG64)(ULONG_PTR)threadID);
	status = PsLookupThreadByThreadId(threadID, &EThread);
	if (!NT_SUCCESS(status)){
		//DbgPrint("get pethread\n");
		return status;
	}
	/*
	CLIENT_ID cid;
	cid.UniqueProcess = NULL;
	cid.UniqueThread = (HANDLE)(ULONG_PTR)(*(PULONG64)inputBuffer);
	
	OBJECT_ATTRIBUTES oa;
	InitializeObjectAttributes(&oa, NULL, OBJ_KERNEL_HANDLE, NULL, NULL);*/
	
	/*HANDLE hThread;
	status = ObOpenObjectByPointer(EThread, OBJ_KERNEL_HANDLE, NULL, THREAD_ALL_ACCESS, *PsThreadType, KernelMode, &hThread);
	if (!NT_SUCCESS(status)){
		ObDereferenceObject(EThread);
		return status;
	}*/
	
	status = PspTerminateThreadByPointer(EThread, 0, TRUE);
	if (!NT_SUCCESS(status)){
		//(hThread);
		DbgPrint("terminate\n");
		ObDereferenceObject(EThread);
		return status;
	}
	ObDereferenceObject(EThread);
	return STATUS_SUCCESS;
}

NTSTATUS DispatchQueryThreads(PVOID inputBuffer, ULONG inputLength, PVOID outputBuffer, ULONG outputLength, PIRP Irp){
	NTSTATUS status;
	
	if (!inputBuffer || !outputBuffer){
		return STATUS_INVALID_USER_BUFFER;
	}
	
	if (inputLength < sizeof(HANDLE) || !outputLength){
		return STATUS_BUFFER_TOO_SMALL;
	}
	
	HANDLE TargetProcessID = *(HANDLE*)inputBuffer;
	
	
	//PEPROCESS targetProcess = NULL;
	//PETHREAD currentThread = NULL;
	//KAPC_STATE apcState;
	//HANDLE targetProcessID = *(HANDLE*)inputBuffer;
	
	/*status = PsLookupProcessByProcessId(targetProcessID, &targetProcess);
	if (!NT_SUCCESS(status)){
		return status;
	}
	
	KeStackAttachProcess(targetProcess, &apcState); //附加到目标进程上下文以操作目标进程地址空间
	//ExAcquirePushLockExclusive(&targetProcess->ThreadListHead.Lock); //加锁
	
	
	//PLIST_ENTRY threadListHead = &targetProcess->ThreadListHead;
	//PLIST_ENTRY threadEntry = threadListHead->Flink;
	
	PQUERY_THREAD_INFORMATION ptinf = (PQUERY_THREAD_INFORMATION)ExAllocatePoolWithTag(PagedPool, sizeof(QUERY_THREAD_INFORMATION)+sizeof(THREAD_INFORMATION)*1023, 'TrQU');
	if (!ptinf){
		KeUnstackDetachProcess(&apcState);
		ObDereferenceObject(targetProcess);
		return STATUS_INSUFFICIENT_RESOURCES;
	}*/
	
	/*
	ULONG index = 0;
	while(threadEntry != threadListHead && index < 1024){
		currentThread = CONTAINING_RECORD(threadEntry, PETHREAD, threadListHead);
		//CLIENT_ID cid = (CLIENT_ID)PsGetThreadId(currentThread);
		ULONG tid = (ULONG)(ULONG_PTR)PsGetThreadId(currentThread);
		
		ptinf->infos[index].TebAddr = currentThread->Tcb.Teb;
		ptinf->infos[index].basePriority = currentThread->Tcb.BasePriority;
		ptinf->infos[index].threadId = tid;
		ptinf->infos[index].State = currentThread->Tccb.State;
		ptinf->infos[index].ThreadEntry = currentThread->Tcb.StartAddress;
		
		index++;
		threadEntry = threadEntry->Flink;
	}
	ptinf->numberOfThreads = index + 1;
	ptinf->sizeOfInfo = sizeof(QUERY_THREAD_INFORMATION) + index * sizeof(THREAD_INFORMATION);*/
	/*
	ULONG index = 0;
	while (index < 1023){
		status = PsGetNextProcessThread(targetProcess, currentThread, &currentThread);
		if (!NT_SUCCESS(status)){
			if (status != STATUS_NO_MORE_ENTRIES){
				KeUnstackDetachProcess(&apcState);
				ExFreePoolWithTag(ptinf, 'TrQU');
				ObDereferenceObject(targetProcess);
				return status;
			}
			break;
		}
		
		//ptinf->infos[index].TebAddr = PsGetThreadTeb(currentThread);
		ptinf->infos[index].basePriority = PsGetThreadBasePriority(currentThread);
		ptinf->infos[index].threadId = (ULONG)(ULONG_PTR)PsGetThreadId(currentThread);
		ptinf->infos[index].State = PsGetThreadState(currentThread);
		ptinf->infos[index].ThreadEntry = (ULONG64)PsGetThreadStartAddress(currentThread);
		
		index++;
		ObDereferenceObject(currentThread);
	}
	ptinf->numberOfThreads = index + 1;
	ptinf->sizeOfInfo = sizeof(QUERY_THREAD_INFORMATION) + index * sizeof(THREAD_INFORMATION);*/
	
	//DbgPrint("CTL QueryThread\n");
	
	ULONG retLength;
	status = ZwQuerySystemInformation(SystemProcessInformation, NULL, 0, &retLength);
	if (status != STATUS_INFO_LENGTH_MISMATCH){
		return status;
	}
	
	//DbgPrint("Need memory: %ld\n", retLength);
	
	PVOID pInfo = ExAllocatePoolWithTag(PagedPool, retLength, 'ThQr');
	if (!pInfo){
		return STATUS_INSUFFICIENT_RESOURCES;
	}
	
	status = ZwQuerySystemInformation(SystemProcessInformation, pInfo, retLength, &retLength);
	if (!NT_SUCCESS(status)){
		ExFreePoolWithTag(pInfo, 'ThQr');
		return status;
	}
	
	PSYSTEM_PROCESS_INFORMATION pCurProcess = (PSYSTEM_PROCESS_INFORMATION)pInfo;
	PSYSTEM_PROCESS_INFORMATION pTargetProcess = NULL;
	while(TRUE){
		if (pCurProcess->UniqueProcessId == TargetProcessID){
			pTargetProcess = pCurProcess;
			break;
		}
		if (!pCurProcess->NextEntryOffset){
			ExFreePoolWithTag(pInfo, 'ThQr');
			return STATUS_INVALID_PARAMETER;
		}
		
		pCurProcess = (PSYSTEM_PROCESS_INFORMATION)((ULONG_PTR)pCurProcess + pCurProcess->NextEntryOffset);
	}
	
	ULONG threadCount = pTargetProcess->NumberOfThreads;
	ULONG size = sizeof(QUERY_THREAD_INFORMATION) + (threadCount - 1) * sizeof(THREAD_INFORMATION);
	
	if (outputLength < size){
		ExFreePoolWithTag(pInfo, 'ThQr');
		return STATUS_BUFFER_TOO_SMALL;
	}
	
	PQUERY_THREAD_INFORMATION ptinf = (PQUERY_THREAD_INFORMATION)ExAllocatePoolWithTag(PagedPool, \
		size, 'ThQU');
	if (!ptinf){
		ExFreePoolWithTag(pInfo, 'ThQr');
		return STATUS_INSUFFICIENT_RESOURCES;
	}
	RtlZeroMemory(ptinf, size);
	
	ptinf->numberOfThreads = threadCount;
	for (ULONG i = 0; i < threadCount; i++){
		PSYSTEM_THREAD_INFORMATION pThread = &pTargetProcess->Threads[i];
		ptinf->infos[i].threadId = (ULONG64)(ULONG_PTR)(pThread->ClientId.UniqueThread);
		ptinf->infos[i].ThreadEntry = (ULONG64)(pThread->StartAddress);
		ptinf->infos[i].basePriority = (LONG32)(pThread->Priority);
		ptinf->infos[i].State = (ULONG)(pThread->State);
		//DbgPrint("%ld 0x%llX %d %d", ptinf->infos[i].threadId, ptinf->infos[i].ThreadEntry, ptinf->infos[i].basePriority, ptinf->infos[i].State);
	}
	
	
	ptinf->sizeOfInfo = size;
	
	/*if (outputLength < ptinf->sizeOfInfo){
		ExFreePoolWithTag(ptinf, 'ThQU');
		ExFreePoolWithTag(pInfo, 'ThQr');
		//KeUnstackDetachProcess(&apcState);
		//ObDereferenceObject(targetProcess);
		return STATUS_BUFFER_TOO_SMALL;
	}*/
	
	memcpy(outputBuffer, ptinf, ptinf->sizeOfInfo);
	Irp->IoStatus.Information = ptinf->sizeOfInfo;
	
	ExFreePoolWithTag(ptinf, 'ThQU');
	ExFreePoolWithTag(pInfo, 'ThQr');
	//KeUnstackDetachProcess(&apcState);
	//ObDereferenceObject(targetProcess);
	
	return STATUS_SUCCESS;
}

NTSTATUS DispatchQueryProcess(PVOID outputBuffer, ULONG outputLength, PIRP Irp){
    NTSTATUS status;
    ULONG retLength;
    
    //DbgPrint("CTL QueryProcess\n");
    
    if (!outputBuffer){
        return STATUS_INVALID_USER_BUFFER;
    }
	
	if (!outputLength || !outputLength){
		return STATUS_BUFFER_TOO_SMALL;
	}
    
    status = ZwQuerySystemInformation(SystemProcessInformation, NULL, 0, &retLength);
    if (status != STATUS_INFO_LENGTH_MISMATCH){
        return status;
    }
    
    //DbgPrint("Need memory: %ld\n", retLength);
    
    PVOID pInfo = ExAllocatePoolWithTag(PagedPool, retLength, 'PsQr');
    if (!pInfo){
        return STATUS_INSUFFICIENT_RESOURCES;
    }
    
    status = ZwQuerySystemInformation(SystemProcessInformation, pInfo, retLength, &retLength);
    if (!NT_SUCCESS(status)){
		ExFreePoolWithTag(pInfo, 'PsQr');
        return status;
    }
    
	
	ULONG64 Initsize = sizeof(QUERY_PROCESS_INFORMATION) + sizeof(PROCESS_INFORMATION)*1023;
    PQUERY_PROCESS_INFORMATION pUserInfo = (PQUERY_PROCESS_INFORMATION)ExAllocatePoolWithTag(PagedPool, Initsize, 'PsRU');
	if (!pUserInfo){
		ExFreePoolWithTag(pInfo, 'PsQr');
		return STATUS_INSUFFICIENT_RESOURCES;
	}
    RtlZeroMemory(pUserInfo, Initsize);
    
    PSYSTEM_PROCESS_INFORMATION pProcIndex = (PSYSTEM_PROCESS_INFORMATION)pInfo;
    //DbgPrint("pid 0 System Idle Process");
    USHORT index = 0;
    PPROCESS_INFORMATION ppinf = &pUserInfo->ProcessInformation[index];
    ppinf->dwPID = 0;
    ppinf->ParentPID = 0;
    memcpy(&ppinf->imageName, L"System Idle Process", 40);
    //RtlStringCbCopyA((PSTR))
    PEPROCESS EProcess;
    do{
    	index++;
    	ppinf = &pUserInfo->ProcessInformation[index];
        pProcIndex = (PSYSTEM_PROCESS_INFORMATION)((ULONG_PTR)pProcIndex + pProcIndex->NextEntryOffset);
        //DbgPrint("Process Name: %-20ws PID: %ld", pProcIndex->ProcessName.Buffer, pProcIndex->ProcessId);
        if (pProcIndex->ImageName.Buffer && pProcIndex->ImageName.Length < 150){
	        status = PsLookupProcessByProcessId(pProcIndex->UniqueProcessId, &EProcess);
	        //if (NT_SUCCESS(status)){
			//DbgPrint("%ld\n", status);
			//}
			//ObReferenceObject(EProcess);
	        ppinf->dwPID = (ULONG64)(ULONG_PTR)pProcIndex->UniqueProcessId;
	        ppinf->ParentPID = (ULONG64)(ULONG_PTR)pProcIndex->ParentProcessId;
	        //DbgPrint("%ld: pid: %ld ParentPID: %ld", index, ppinf->dwPID, ppinf->ParentPID);
	        memcpy(&ppinf->imageName, pProcIndex->ImageName.Buffer, pProcIndex->ImageName.Length);
	        ppinf->imageName[63] = 0x0000;
	        //ppinf->SessionID = (ULONG64)pProcIndex->SessionId;
	        //ppinf->NumberOfThreads = (ULONG64)pProcIndex->NumberOfThreads;
	        //DbgPrint("%ld %ld", pProcIndex->SessionId, pProcIndex->NumberOfThreads);
			//PsGetProcessSessionId(EProcess, &ppinf->SessionID);
			//ppinf->SessionID = (ULONG64)pProcIndex->SessionId;
			ppinf->EProcessAddr = (ULONG64)(ULONG_PTR)EProcess;
			//DbgPrint("0x%016llX\n", (ULONG64)(ULONG_PTR)EProcess);
			//ObDereferenceObject(EProcess);
			
        }
    } while(pProcIndex->NextEntryOffset != 0);
	
	SIZE_T size = sizeof(QUERY_PROCESS_INFORMATION) + index*sizeof(PROCESS_INFORMATION);
	
	if (outputLength < size){
		ExFreePoolWithTag(pInfo, 'PsQr');
		ExFreePoolWithTag(pUserInfo, 'PsQU');
		return STATUS_BUFFER_TOO_SMALL;
	}
	
	pUserInfo->numberOfProcess = index+1;
	pUserInfo->sizeOfInfo = size;
	
	memcpy(outputBuffer, pUserInfo, size);
	Irp->IoStatus.Information = size;
    
	ExFreePoolWithTag(pInfo, 'PsQr');
	ExFreePoolWithTag(pUserInfo, 'PsQU');
	
    return STATUS_SUCCESS;
}

OB_PREOP_CALLBACK_STATUS PreOpenProcessCallback(PVOID RegistrationContext, POB_PRE_OPERATION_INFORMATION OperationInformation){
	PEPROCESS TargetProcess = (PEPROCESS)OperationInformation->Object;
	//DbgPrint("OpenProcess Callback!\n");
	if (!TargetProcess || !OperationInformation) return (OB_PREOP_CALLBACK_STATUS)OB_PREOP_SUCCESS;
	if (!RegistrationContext) return (OB_PREOP_CALLBACK_STATUS)OB_PREOP_SUCCESS;
	ULONG MaxCount = ((PCALLBACK_CONTEXT)(RegistrationContext))->OpenProcessCallbackContextUsedCount;
	if (!MaxCount) return OB_PREOP_SUCCESS;
	
	//DbgPrint("a");
	
	if (OperationInformation->ObjectType != *PsProcessType ||
		OperationInformation->Operation != OB_OPERATION_HANDLE_CREATE) {
		return (OB_PREOP_CALLBACK_STATUS)OB_PREOP_SUCCESS;
	}
	
	ULONG UniqueProcessID;
	UniqueProcessID = (ULONG64)(ULONG_PTR)PsGetProcessId(TargetProcess);
	
	ULONG DisallowProcessID; //= *(PULONG)RegistrationContext;
	
	for (ULONG i=0;i<MaxCount;i++){
		DisallowProcessID = ((PCALLBACK_CONTEXT)(RegistrationContext))->ProcessIDs[i];
		if (UniqueProcessID == DisallowProcessID){
			//DbgPrint("Disallow OpenProcess Caller: %ld Target: %ld\n", PsGetCurrentProcessId(), UniqueProcessID);
			OperationInformation->Parameters->CreateHandleInformation.DesiredAccess = 0;
		}
	}
	/*
	if (UniqueProcessID == DisallowProcessID){
		DbgPrint("Disallow OpenProcess Caller: %ld Target: %ld\n", PsGetCurrentProcessId(), UniqueProcessID);
		//return (OB_PREOP_CALLBACK_STATUS)OB_PREOP_DISALLOW;
		OperationInformation->Parameters->CreateHandleInformation.DesiredAccess = 0;
	}*/
	return (OB_PREOP_CALLBACK_STATUS)OB_PREOP_SUCCESS;
}

NTSTATUS DispatchSuccess(PDEVICE_OBJECT DeviceObject, PIRP Irp){
	UNREFERENCED_PARAMETER(DeviceObject);
	
	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	
	return STATUS_SUCCESS;
}
/*
NTSTATUS DispatchOpenProcess(PVOID inputBuffer, ULONG inputLength, PVOID outputBuffer, ULONG outputLength, PIRP Irp){
	//     Ψ?    ID           ? ?  
	NTSTATUS status;
	
	//DbgPrint("CTL OpenProcess\n");
	
	if (!inputBuffer || !outputBuffer) {
		return STATUS_INVALID_USER_BUFFER;
	}
	if (inputLength < sizeof(ULONG) || outputLength < sizeof(HANDLE)){
		return STATUS_BUFFER_TOO_SMALL;
	}
	
	//if ()
	
	ULONG dwPID = *((PULONG)inputBuffer);
	HANDLE hProcess;
	
	OBJECT_ATTRIBUTES oa;
	CLIENT_ID cid = {(HANDLE)(ULONG_PTR)dwPID, NULL};
	InitializeObjectAttributes(&oa, NULL, 0, NULL, NULL);
	
	status = ZwOpenProcess(&hProcess, PROCESS_ALL_ACCESS, &oa, &cid);
	if (!NT_SUCCESS(status)){
		return status;
	}
	
	//DbgPrint("HANDLE: 0x%p\n", hProcess);
	
	status = ZwDuplicateObject(NtCurrentProcess(), hProcess, NtCurrentProcess(), \
		(PHANDLE)outputBuffer, 0, 0, DUPLICATE_SAME_ACCESS);
	Irp->IoStatus.Information = sizeof(HANDLE);
	if (!NT_SUCCESS(status)){
		ZwClose(hProcess);
		return status;
	}
	return ZwClose(hProcess);
}*/

NTSTATUS DispatchTerminateProcess(PVOID inputBuffer, ULONG inputLength){
	NTSTATUS status;
	
	//DbgPrint("CTL TeminateProcess\n");
	
	if (!inputBuffer){
		return STATUS_INVALID_USER_BUFFER;
	}
	
	if (inputLength < sizeof(ULONG64)){
		return STATUS_BUFFER_TOO_SMALL;
	}
	
	ULONG64 dwPID = *((PULONG64)inputBuffer);
	HANDLE hProcess;
	
	OBJECT_ATTRIBUTES oa;
	CLIENT_ID cid = {(HANDLE)(ULONG_PTR)dwPID, NULL};
	InitializeObjectAttributes(&oa, NULL, 0, NULL, NULL);
	
	status = ZwOpenProcess(&hProcess, PROCESS_ALL_ACCESS, &oa, &cid);
	if (!NT_SUCCESS(status)){
		return status;
	}
	
	status = ZwTerminateProcess(hProcess, 0);
	ZwClose(hProcess);
	return status;
}

NTSTATUS ProtectProcess(PDEVICE_OBJECT DeviceObject){
	NTSTATUS status;
	
	//DbgPrint("ProtectProcess\n");
	
	//pDevExt->OpenProcessCallbackContextSize = sizeof(ULONG)*512;
	PDEVICE_EXTENSION pDevExt = (PDEVICE_EXTENSION)DeviceObject->DeviceExtension;
	pDevExt->OpenProcessCallbackContext = (PCALLBACK_CONTEXT)ExAllocatePoolWithTag(NonPagedPool, sizeof(CALLBACK_CONTEXT), 'PsCR');
	if (!pDevExt->OpenProcessCallbackContext){
		pDevExt->OpenProcessCallbackContext = NULL;
		return STATUS_INSUFFICIENT_RESOURCES;
	}
	RtlZeroMemory(pDevExt->OpenProcessCallbackContext, sizeof(CALLBACK_CONTEXT));
	pDevExt->OpenProcessCallbackContext->OpenProcessCallbackContextUsedCount = 0;
	//*(PULONG)pDevExt->OpenProcessCallbackContext = dwPID;
	
	OB_OPERATION_REGISTRATION opr[1];
	opr[0].ObjectType = PsProcessType;
	opr[0].Operations = OB_OPERATION_HANDLE_CREATE;
	opr[0].PreOperation = &PreOpenProcessCallback;
	opr[0].PostOperation = NULL;
	OB_CALLBACK_REGISTRATION reg;
	reg.Version = OB_FLT_REGISTRATION_VERSION;
	reg.OperationRegistrationCount = 1;
	RtlInitUnicodeString(&reg.Altitude, L"321000.0");
	reg.RegistrationContext = pDevExt->OpenProcessCallbackContext;
	reg.OperationRegistration = opr;
	
	status = ObRegisterCallbacks(&reg, (PHANDLE)&pDevExt->OpenProcessCallbackHandle);
	if (!NT_SUCCESS(status)){
		ExFreePoolWithTag(pDevExt->OpenProcessCallbackContext, 'PsCR');
		pDevExt->OpenProcessCallbackContext = NULL;
	}
	//DbgPrint("status: 0x%08X\n", status);
	return status;
}

NTSTATUS DispatchChangeProtectProcess(PVOID inputBuffer, ULONG inputLength, PDEVICE_OBJECT DeviceObject){
	//DbgPrint("CTL ProtectProcess\n");
	
	if (!inputBuffer){
		return STATUS_INVALID_USER_BUFFER;
	}
	
	if (inputLength < sizeof(ULONG)){
		return STATUS_BUFFER_TOO_SMALL;
	}
	
	PDEVICE_EXTENSION pDevExt = (PDEVICE_EXTENSION)DeviceObject->DeviceExtension;
	PULONG64 pPIDContext = (PULONG64)((ULONG_PTR)pDevExt->OpenProcessCallbackContext->ProcessIDs+0);
	pDevExt->OpenProcessCallbackContext->OpenProcessCallbackContextUsedCount = 1;
	*pPIDContext = *(PULONG64)inputBuffer;
	
	return STATUS_SUCCESS;
}

NTSTATUS DispatchDeviceControl(PDEVICE_OBJECT DeviceObject, PIRP Irp){
	//UNREFERENCED_PARAMETER(DeviceObject);
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(Irp);
	NTSTATUS status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;
	//ULONG_PTR info = 0;
	
	PVOID inputBuffer = Irp->AssociatedIrp.SystemBuffer;
	ULONG inputLength = stack->Parameters.DeviceIoControl.InputBufferLength;
	
	PVOID outputBuffer = Irp->AssociatedIrp.SystemBuffer;
	ULONG outputLength = stack->Parameters.DeviceIoControl.OutputBufferLength;
	
	//DbgPrint("DeviceIoControl: inputBuffer 0x%08X size: 0x%08X outputBuffer 0x%08X size: 0x%08X PID: %ld\n", (ULONG_PTR)inputBuffer, inputLength, (ULONG_PTR)outputBuffer, outputLength, PsGetCurrentProcessId());
	
	switch (stack->Parameters.DeviceIoControl.IoControlCode) {
		case CTL_CODE_OPEN_PROCESS: {
			//status = DispatchOpenProcess(inputBuffer, inputLength, outputBuffer, outputLength, Irp);
			status = STATUS_UNSUCCESSFUL;
			break;
		}
		case CTL_CODE_TERMINATE_PROCESS: {
			status = DispatchTerminateProcess(inputBuffer, inputLength);
			break;
		}
		case CTL_CODE_PROTECT_PROCESS: {
			status = DispatchChangeProtectProcess(inputBuffer, inputLength, DeviceObject);
			break;
		}
		case CTL_CODE_QUERY_PROCESS: {
		    status = DispatchQueryProcess(outputBuffer, outputLength, Irp);
		    break;
		}
		case CTL_CODE_QUERY_THREADS: {
			status = DispatchQueryThreads(inputBuffer, inputLength, outputBuffer, outputLength, Irp);
			break;
		}
		case CTL_CODE_TERMINATE_THREAD: {
			status = DispatchTerminateThread(inputBuffer, inputLength);
			break;
		}
		case CTL_CODE_QUERY_HANDLE: {
			status = DispatchQueryProcessHandle(inputBuffer, inputLength, outputBuffer, outputLength, Irp);
			break;
		}
		case CTL_CODE_CLOSE_HANDLE: {
			status = DispatchCloseHandle(inputBuffer, inputLength);
			//status = STATUS_UNSUCCESSFUL;
			break;
		}
		case CTL_CODE_SUSPEND_PROCESS: {
			//status = DispatchSuspendProcess(inputBuffer, inputLength);
			status = STATUS_UNSUCCESSFUL;
			break;
		}
		case CTL_CODE_RESUME_PROCESS: {
			//status = DispatchResumeProcess(inputBuffer, inputLength);
			status = STATUS_UNSUCCESSFUL;
			break;
		}
		case CTL_CODE_QUERY_SSDT_DATA: {
			status = DispatchQuerySSDTData(inputBuffer, inputLength, outputBuffer, outputLength, Irp);
			break;
		}
		case CTL_CODE_GET_NTOSKRNL_BASE: {
			//status = DispatchGetNtOsKrnlBase(outputBuffer, outputLength, Irp);
			status = STATUS_UNSUCCESSFUL;
			break;
		}
		case CTL_CODE_QUERY_SSSDT_DATA: {
			status = DispatchQuerySSSDTData(inputBuffer, inputLength, outputBuffer, outputLength, Irp);
			break;
		}
		case CTL_CODE_QUERY_OBJECT_TYPE_DATA: {
			status = DispatchQueryObjectTableData(outputBuffer, outputLength, Irp);
			break;
		}
		case CTL_CODE_QUERY_OBJECT_TYPE_CALLBACK: {
			status = DispatchQueryObjectCallbacks(outputBuffer, outputLength, Irp);
			break;
		}
		default:{
			status = STATUS_INVALID_DEVICE_REQUEST;
		}
	}
	
	Irp->IoStatus.Status = status;
	//DbgPrint("status: 0x%08X\n", status);
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return status;
}

NTSTATUS DispatchClose(PDEVICE_OBJECT DeviceObject, PIRP Irp){
	PDEVICE_EXTENSION pDevExt = (PDEVICE_EXTENSION)DeviceObject->DeviceExtension;
	
	InterlockedExchange16(&pDevExt->OpenCount, 0);
	
	//DbgPrint("PID: %ld Close this driver\n", PsGetCurrentProcessId());
	
	Irp->IoStatus.Status = STATUS_SUCCESS;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	
	//PDRIVER_OBJECT DriverObject = DeviceObject->DriverObject;
	//DriverUnload(DriverObject);
	//return IoUnregisterDriver(DriverObject);
	return STATUS_SUCCESS;
}

NTSTATUS DispatchCreate(PDEVICE_OBJECT DeviceObject, PIRP Irp){
	PDEVICE_EXTENSION pDevExt = (PDEVICE_EXTENSION)DeviceObject->DeviceExtension;
	
	NTSTATUS status;
	//pDevExt->SelfPID = NtCurrentProcess();
	//DbgPrint("PID: %ld Open this driver\n", PsGetCurrentProcessId());
	DWORD32 curCount = InterlockedCompareExchange16(&pDevExt->OpenCount, 1, 0);
	if (curCount != 0){
		status = STATUS_ACCESS_DENIED;
	} else {
		status = STATUS_SUCCESS;
	}
	Irp->IoStatus.Status = status;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return status;
}



/**
 * 查找PspTerminateThreadByPointer在PsTerminateSystemThread中的引用以获取地址来结束线程
 * 特征码 E8 ?? ?? ?? ??  call 		PspTerminateThreadByPointer
 *        90     |         nop	 
 *          系统动态生成 -> 写入iOffset -> iOffset+i (偏移+所在地址=实际地址)
 * win7x64 win7x86 win10x86过
 **/
NTSTATUS GetPspTerminateThreadByPointer(){
	UNICODE_STRING PspTerminateSystemThreadName = RTL_CONSTANT_STRING(L"PsTerminateSystemThread");
	PVOID pPsTerminateSystemThread = MmGetSystemRoutineAddress(&PspTerminateSystemThreadName);
	if (!pPsTerminateSystemThread){
		return STATUS_UNSUCCESSFUL;
	}
	
	UINT8* StartSearchAddress = (UINT8*)pPsTerminateSystemThread;
	UINT8* EndSearchAddress = StartSearchAddress + 0x500;
	UINT8* i = NULL;
	UINT8 v1 = 0, v2 = 0; //v1 -> E8 call特征   v2 -> 90 nop特征
	LONG32 iOffset = 0;
	//_PspTerminateThreadByPointer PspTerminateThreadByPointer = NULL;
	PspTerminateThreadByPointer = NULL;
	
	for (i=StartSearchAddress;i<EndSearchAddress;i++){
		if (MmIsAddressValid(i) && MmIsAddressValid(i+5)){ //i -> v1  (i+5) -> v2
			v1 = *i;
			v2 = *(i + 5); //dangerous!!!!!!
			if (v1 == 0xE8 && v2 == 0x90){ //匹配特征码 E8 ?? ?? ?? ?? 90
				RtlCopyMemory(&iOffset, i+1, 4);
				PspTerminateThreadByPointer = (_PspTerminateThreadByPointer)(iOffset+(ULONG64)i+5);
				//															  偏移      基址 指令长度
				DbgPrint("PspTerminateThreadByPointer Addr: 0x%016llX\ne", (ULONG64)PspTerminateThreadByPointer);
				return STATUS_SUCCESS;
			}
		}
	}
	
	/*if (PspTerminateThreadByPointer == NULL) { 
		DbgPrint("Get Function2 failed!\n");
		DriverUnload(DriverObject);
		return STATUS_UNSUCCESSFUL;
	}*/
	
	return STATUS_UNSUCCESSFUL;
}

/**
 * 在KiSystemCall64中查找SSDT和SSSDT
 * 特征码 4C 8D 15 ?? ?? ?? ??   lea r10, SSDT
 *        4C 8D 1D ?? ?? ?? ??   lea r11, SSSDT
 **/
NTSTATUS GetSSDTandSSSDT(){
	UINT8* StartAddress = (UINT8*)__readmsr(0xC0000082);
	DbgPrint("MSR Address: 0x%0p\n", StartAddress);
	if (!StartAddress){
		//DbgPrint("1\n");
		return STATUS_UNSUCCESSFUL;
	}
	
	UINT8* EndAddress = StartAddress + 0x500;
	UINT8* i = NULL;
	UINT8 v1, v2, v3, v8, v9, v10;
	UINT32 v4, v11;
	//AddrSSDT = NULL;
	//AddrSSSDT = NULL;
	
	for(i=StartAddress;i<EndAddress;i++){
		/*if(MmIsAddressValid(i) && MmIsAddressValid(i+14)){
			v1 = *(i+0);
			v2 = *(i+1);
			v3 = *(i+2);
			v8 = *(i+7);
			v9 = *(i+8);
			v10 = *(i+9);
			
			if (v1 == 0x4C && v2 == 0x8D && v3 == 0x15 && v8 == 0x4C && v9 == 0x8D && v10 == 0x1D){
				v4 = *((UINT32*)(i+3));
				v11 = *((UINT32*)(i+10));
				AddrSSDT = (PSERVICE_DESCRIPTOR_TABLE)((ULONG64)(ULONG_PTR)i + v4 + 7);
				AddrSSSDT = (PSERVICE_DESCRIPTOR_TABLE)((ULONG64)(ULONG_PTR)i + v11 + 14);
		
				DbgPrint("SSDT: 0x%0p SSSDT: 0x%0p\n", (ULONG_PTR)AddrSSDT, (ULONG_PTR)AddrSSSDT);
				return STATUS_SUCCESS;
			}
		}*/
		if(MmIsAddressValid(i) && MmIsAddressValid(i+14)){
			v1 = *(i+0);
			if (v1 != 0x4C) continue;
			
			v2 = *(i+1);
			if (v2 != 0x8D) continue;
			
			v3 = *(i+2);
			v8 = *(i+7);
			v9 = *(i+8);
			v10 = *(i+9);
			
			if (v1 == 0x4C && v2 == 0x8D && v3 == 0x15 && v8 == 0x4C && v9 == 0x8D && v10 == 0x1D){
				v4 = *((UINT32*)(i+3));
				v11 = *((UINT32*)(i+10));
				AddrSSDT = (PSERVICE_DESCRIPTOR_TABLE)((ULONG64)(ULONG_PTR)i + v4 + 7);
				AddrSSSDT = (PSERVICE_DESCRIPTOR_TABLE)((ULONG64)(ULONG_PTR)i + v11 + 14);
		
				DbgPrint("SSDT: 0x%0p SSSDT: 0x%0p\n", (ULONG_PTR)AddrSSDT, (ULONG_PTR)AddrSSSDT);
				return STATUS_SUCCESS;
			}
		}
	}
	return STATUS_UNSUCCESSFUL;
}

/**
 * 在ObGetObjectType中查找ObTypeIndexTable
 * ObTypeIndexTable: OBJECT_TYPE ObTypeIndexTable[0x100]
 * 即可解析
 * 
 * 特征码 48 8D 0D ?? ?? ?? ??   lea rcx, [ObTypeIndexTable]
 *        48 8B 04 C1            mov rcx, qword ptr [rcx + rax*8]
 * 	kd> u ObGetObjectType
	 nt!ObGetObjectType:
	 fffff800`041317e0 0fb641e8        movzx   eax,byte ptr [rcx-18h]
	 fffff800`041317e4 488d0d15dbefff  lea     rcx,[nt!ObTypeIndexTable (fffff800`0402f300)]
	 fffff800`041317eb 488b04c1        mov     rax,qword ptr [rcx+rax*8]
	 fffff800`041317ef c3              ret
 **/
NTSTATUS GetObTypeIndexTable(){
	UNICODE_STRING ObGetObjectTypeName = RTL_CONSTANT_STRING(L"ObGetObjectType");
	PVOID pObGetObjectType = MmGetSystemRoutineAddress(&ObGetObjectTypeName);
	if (!pObGetObjectType){
		return STATUS_UNSUCCESSFUL;
	}
	
	UINT8* StartSearchAddress = (UINT8*)pObGetObjectType;
	UINT8* EndSearchAddress = StartSearchAddress + 0x20; //足够
	LONG32 iOffset = 0;
	UINT8 v1, v2, v3;
	UINT32 v4;
	
	for(UINT8* i = StartSearchAddress;i<=EndSearchAddress;i++){
		v1 = *i;
		if (v1 != 0x48) continue;
		
		v2 = *(i+1);
		v3 = *(i+2);
		v4 = *(UINT32*)(i+7);
		
		if(v2 == 0x8d && v3 == 0x0d && v4 == 0xc1048b48){
			iOffset = *(LONG32*)(i+3);
			ObTypeIndexTable = (ULONG64)i+iOffset+7;
			DbgPrint("ObTypeIndexTableAddr: 0x%0p\n", ObTypeIndexTable);
			//return STATUS_SUCCESS;
		}
	}
	
	if (ObTypeIndexTable == NULL){
		return STATUS_UNSUCCESSFUL;
	}
	
	return STATUS_SUCCESS;
}

/**
 * 在PsLookupProcessByProcessId中查找PspCidTable
 * PspCidTable: HANDLE_TABLE *PspCidTable
 * 即可解析
 * 
 * 特征码 48 8B 0D ?? ?? ?? ??   mov rcx, qword ptr [PspCidTable]
 *        E8 ?? ?? ?? ??		 call ExMapHandleToPointer
 * kd> u
   nt!PsLookupProcessByProcessId+0x21:
   fffff800`0414f771 4533e4          xor     r12d,r12d
   fffff800`0414f774 488bea          mov     rbp,rdx
   fffff800`0414f777 66ff8fc4010000  dec     word ptr [rdi+1C4h]
   fffff800`0414f77e 498bdc          mov     rbx,r12
   fffff800`0414f781 488bd1          mov     rdx,rcx
   fffff800`0414f784 488b0dddd3edff  mov     rcx,qword ptr [nt!PspCidTable (fffff800`0402cb68)]
   fffff800`0414f78b e8702a0400      call    nt!ExMapHandleToPointer (fffff800`04192200)
   fffff800`0414f790 458d6c2401      lea     r13d,[r12+1]
 **/
NTSTATUS GetPspCidTable(){
	UNICODE_STRING PsLookupProcessByProcessIdName = RTL_CONSTANT_STRING(L"PsLookupProcessByProcessId");
	PVOID pPsLookupProcessByProcessId = MmGetSystemRoutineAddress(&PsLookupProcessByProcessIdName);
	if (!pPsLookupProcessByProcessId){
		return STATUS_UNSUCCESSFUL;
	}
	
	UINT8* StartSearchAddress = (UINT8*)pPsLookupProcessByProcessId;
	UINT8* EndSearchAddress = StartSearchAddress + 0x50; //足够
	LONG32 iOffset = 0;
	UINT8 v1, v2, v3, v4;
	
	for(UINT8* i = StartSearchAddress;i<=EndSearchAddress;i++){
		v1 = *i;
		if (v1 != 0x48) continue;
		
		v2 = *(i+1);
		v3 = *(i+2);
		v4 = *(i+7);
		
		if(v2 == 0x8b && v3 == 0x0d && v4 == 0xe8){
			iOffset = *(LONG32*)(i+3);
			PspCidTable = (ULONG64)i+iOffset+7;
			DbgPrint("PspCidTableAddr: 0x%0p\n", PspCidTable);
			//return STATUS_SUCCESS;
		}
	}
	
	if (PspCidTable == NULL){
		return STATUS_UNSUCCESSFUL;
	}
	
	return STATUS_SUCCESS;
}

NTSTATUS GetSSDTandSSSDTFuncByIndex(BOOLEAN IsSSDTFunc, ULONG32 Index, PVOID* pFuncAddr){
	if (IsSSDTFunc){
		if (!AddrSSDT || !MmIsAddressValid(AddrSSDT)){
			return STATUS_UNSUCCESSFUL;
		}
		
		if (!AddrSSDT->ServiceTableBase || !MmIsAddressValid(AddrSSDT->ServiceTableBase)){
			return STATUS_UNSUCCESSFUL;
		}
		
		ULONG ServiceCount = AddrSSDT->NumberOfServices;
		if (Index > ServiceCount){
			return STATUS_UNSUCCESSFUL;
		}
		
		*pFuncAddr = (PVOID)((LONG64)AddrSSDT->ServiceTableBase+((LONG)(AddrSSDT->ServiceTableBase[Index])>>4));
		
		return STATUS_SUCCESS;
	} else {
		if (!AddrSSSDT || !MmIsAddressValid(AddrSSSDT)){
			return STATUS_UNSUCCESSFUL;
		}
		
		if (!AddrSSSDT[1].ServiceTableBase || !MmIsAddressValid(AddrSSSDT[1].ServiceTableBase)){
			return STATUS_UNSUCCESSFUL;
		}
		
		ULONG ServiceCount = AddrSSSDT[1].NumberOfServices;
		if (Index > ServiceCount){
			return STATUS_UNSUCCESSFUL;
		}
		
		*pFuncAddr = (PVOID)((LONG64)AddrSSSDT[1].ServiceTableBase+((LONG)(AddrSSSDT[1].ServiceTableBase[Index])>>4));
		
		return STATUS_SUCCESS;
	}
}

NTSTATUS GetFunctionLengthMax(PVOID FuncAddr, PULONG32 pLength, ULONG32 MaxSearchSize){
	INT8 opcodes[6] = {0xC2, 0xC3, 0xCA, 0xCB, 0xE9, 0xEB};
				   // ret16 ret   retf16 retf jmp32 jmp8 jmprm(more check)0xFF
	
	*pLength = 0;
	
	for (ULONG_PTR i=(ULONG64)FuncAddr;i<(ULONG64)FuncAddr+MaxSearchSize;i++){
		if (*(PUCHAR)i == 0xFF){
			if (MmIsAddressValid((PVOID)(i+1))){
				INT8 regOpcode = (*(PUCHAR)(i+1)&0x38)>>3;
				if (regOpcode == 4 || regOpcode == 5){
					return STATUS_SUCCESS;
				}
			}
		}
		for (int j=0;j<6;j++){ //no jmprm
			if (MmIsAddressValid((PVOID)i)){
				//DbgPrint("opcode: 0x%04X\nneed: opcodes: 0x%04X", *(INT8*)i, opcodes[j]);
				if (*(INT8*)i == (INT8)opcodes[j]){
					return STATUS_SUCCESS;
				}
			}
		}

		*pLength += 1;
	}
	
	//*pLength = 0;
	return STATUS_SUCCESS;
}
/*
NTSTATUS HookFunc(PVOID FuncAddr, PVOID NewProc){
	KIRQL oldIRQL;
	KeRaiseIrql(DISPATCH_LEVEL, &oldIRQL);
	ULONG64 cr0 = __readcr0();
	__writecr0(cr0 & ~0x10000);
	
	ULONG64 data = (ULONG64)NewProc;
	INT8 shellcode[11] = {0x68, 0x00, 0x00, 0x00, 0x00, 0x68, 0x00, 0x00, 0x00, 0x00, 0xC3};
	shellcode[1] = *(INT8*)((ULONG64)&data+3); //4
	shellcode[2] = *(INT8*)((ULONG64)&data+2); //3
	shellcode[3] = *(INT8*)((ULONG64)&data+1); //2
	shellcode[4] = *(INT8*)((ULONG64)&data+0); //1
	shellcode[6] = *(INT8*)((ULONG64)&data+7); //8
	shellcode[7] = *(INT8*)((ULONG64)&data+6); //7
	shellcode[8] = *(INT8*)((ULONG64)&data+5); //6
	shellcode[9] = *(INT8*)((ULONG64)&data+4); //5
	
	memcpy(FuncAddr, shellcode, 11);
	
	__writecr0(cr0);
	KeLowerIrql(oldIRQL)
	return STATUS_SUCCESS;
}
*/

NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath) {
	DbgPrint("Driver start\n");
	
	DriverObject->DriverUnload = DriverUnload;
	DriverObject->Flags |= DO_BUFFERED_IO;
	UNREFERENCED_PARAMETER(RegistryPath);
	
	PDEVICE_OBJECT deviceObject;
	UNICODE_STRING deviceName, symbolLinkName;
	RtlInitUnicodeString(&deviceName, L"\\Device\\SuperRun");
	RtlInitUnicodeString(&symbolLinkName, L"\\??\\KTJLL_SuperRun");
	
	NTSTATUS status;
	status = IoCreateDevice(DriverObject, sizeof(DEVICE_EXTENSION), &deviceName, FILE_DEVICE_UNKNOWN, \
	0, TRUE, &deviceObject);
	
	if (!NT_SUCCESS(status)){
		DbgPrint("IoCreateDevice failed: 0x%08X\n", status);
		return status;
	}
	
	DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DispatchDeviceControl;
	DriverObject->MajorFunction[IRP_MJ_CREATE] = DispatchCreate;
	DriverObject->MajorFunction[IRP_MJ_CLEANUP] = DispatchSuccess;
	DriverObject->MajorFunction[IRP_MJ_CLOSE] = DispatchClose;
	
	status = IoCreateSymbolicLink(&symbolLinkName, &deviceName);
	if (!NT_SUCCESS(status)){
		DbgPrint("IoCreateSymbolicLink failed: 0x%08X\n", status);
		IoDeleteDevice(deviceObject);
		return status;
	}
	
	PDEVICE_EXTENSION pDevExt = (PDEVICE_EXTENSION)deviceObject->DeviceExtension;
	RtlZeroMemory(pDevExt, sizeof(DEVICE_EXTENSION));
	
	ULONG_PTR pDrvSection = (ULONG_PTR)DriverObject->DriverSection;
	*(PULONG)(pDrvSection + 0x68) |= 0x20;	// very dangerous -> ProcessProtect!!!!!!!!!!!!!!!
	
	status = ProtectProcess(DriverObject->DeviceObject);
	if (!NT_SUCCESS(status)){
		DbgPrint("Begin register function failed: 0x%08X\n", status);
		DriverUnload(DriverObject);
		return status;
	}
	
	UNICODE_STRING ZwQuerySystemInformationName = RTL_CONSTANT_STRING(L"ZwQuerySystemInformation");
	ZwQuerySystemInformation = (_ZwQuerySystemInformation)MmGetSystemRoutineAddress(&ZwQuerySystemInformationName);
	if (!ZwQuerySystemInformation){
		DbgPrint("Get Function failed!\n");
		DriverUnload(DriverObject);
		return STATUS_UNSUCCESSFUL;
	}
	
	UNICODE_STRING ExEnumHandleTableName = RTL_CONSTANT_STRING(L"ExEnumHandleTable");
	ExEnumHandleTable = (_ExEnumHandleTable)MmGetSystemRoutineAddress(&ExEnumHandleTableName);
	if (!ExEnumHandleTable){
		DriverUnload(DriverObject);
		return STATUS_UNSUCCESSFUL;
	}
	
	if (!NT_SUCCESS(GetPspTerminateThreadByPointer())){
		DriverUnload(DriverObject);
		return STATUS_UNSUCCESSFUL;
	}
	
	if (!NT_SUCCESS(GetSSDTandSSSDT())){
		DriverUnload(DriverObject);
		return STATUS_UNSUCCESSFUL;
	}
	
	if (!NT_SUCCESS(GetNtOsKrnlAndWin32kBase(DriverObject))){
		DriverUnload(DriverObject);
		return STATUS_UNSUCCESSFUL;
	}
	
	if (!NT_SUCCESS(GetPspCidTable())){
		DriverUnload(DriverObject);
		return STATUS_UNSUCCESSFUL;
	}
	
	if (!NT_SUCCESS(GetObTypeIndexTable())){
		DriverUnload(DriverObject);
		return STATUS_UNSUCCESSFUL;
	}
	
	//EnumPspCidTable();
	
	return STATUS_SUCCESS;
}
