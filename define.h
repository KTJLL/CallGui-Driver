#include <ntddk.h>

#define PROCESS_PROTECT_COUNT	512

typedef enum _SYSTEM_INFORMATION_CLASS
{
	SystemBasicInformation = 0x0,
	SystemProcessorInformation = 0x1,
	SystemPerformanceInformation = 0x2,
	SystemTimeOfDayInformation = 0x3,
	SystemPathInformation = 0x4,
	SystemProcessInformation = 0x5,
	SystemCallCountInformation = 0x6,
	SystemDeviceInformation = 0x7,
	SystemProcessorPerformanceInformation = 0x8,
	SystemFlagsInformation = 0x9,
	SystemCallTimeInformation = 0xa,
	SystemModuleInformation = 0xb,
	SystemLocksInformation = 0xc,
	SystemStackTraceInformation = 0xd,
	SystemPagedPoolInformation = 0xe,
	SystemNonPagedPoolInformation = 0xf,
	SystemHandleInformation = 0x10,
	SystemObjectInformation = 0x11,
	SystemPageFileInformation = 0x12,
	SystemVdmInstemulInformation = 0x13,
	SystemVdmBopInformation = 0x14,
	SystemFileCacheInformation = 0x15,
	SystemPoolTagInformation = 0x16,
	SystemInterruptInformation = 0x17,
	SystemDpcBehaviorInformation = 0x18,
	SystemFullMemoryInformation = 0x19,
	SystemLoadGdiDriverInformation = 0x1a,
	SystemUnloadGdiDriverInformation = 0x1b,
	SystemTimeAdjustmentInformation = 0x1c,
	SystemSummaryMemoryInformation = 0x1d,
	SystemMirrorMemoryInformation = 0x1e,
	SystemPerformanceTraceInformation = 0x1f,
	SystemObsolete0 = 0x20,
	SystemExceptionInformation = 0x21,
	SystemCrashDumpStateInformation = 0x22,
	SystemKernelDebuggerInformation = 0x23,
	SystemContextSwitchInformation = 0x24,
	SystemRegistryQuotaInformation = 0x25,
	SystemExtendServiceTableInformation = 0x26,
	SystemPrioritySeperation = 0x27,
	SystemVerifierAddDriverInformation = 0x28,
	SystemVerifierRemoveDriverInformation = 0x29,
	SystemProcessorIdleInformation = 0x2a,
	SystemLegacyDriverInformation = 0x2b,
	SystemCurrentTimeZoneInformation = 0x2c,
	SystemLookasideInformation = 0x2d,
	SystemTimeSlipNotification = 0x2e,
	SystemSessionCreate = 0x2f,
	SystemSessionDetach = 0x30,
	SystemSessionInformation = 0x31,
	SystemRangeStartInformation = 0x32,
	SystemVerifierInformation = 0x33,
	SystemVerifierThunkExtend = 0x34,
	SystemSessionProcessInformation = 0x35,
	SystemLoadGdiDriverInSystemSpace = 0x36,
	SystemNumaProcessorMap = 0x37,
	SystemPrefetcherInformation = 0x38,
	SystemExtendedProcessInformation = 0x39,
	SystemRecommendedSharedDataAlignment = 0x3a,
	SystemComPlusPackage = 0x3b,
	SystemNumaAvailableMemory = 0x3c,
	SystemProcessorPowerInformation = 0x3d,
	SystemEmulationBasicInformation = 0x3e,
	SystemEmulationProcessorInformation = 0x3f,
	SystemExtendedHandleInformation = 0x40,
	SystemLostDelayedWriteInformation = 0x41,
	SystemBigPoolInformation = 0x42,
	SystemSessionPoolTagInformation = 0x43,
	SystemSessionMappedViewInformation = 0x44,
	SystemHotpatchInformation = 0x45,
	SystemObjectSecurityMode = 0x46,
	SystemWatchdogTimerHandler = 0x47,
	SystemWatchdogTimerInformation = 0x48,
	SystemLogicalProcessorInformation = 0x49,
	SystemWow64SharedInformationObsolete = 0x4a,
	SystemRegisterFirmwareTableInformationHandler = 0x4b,
	SystemFirmwareTableInformation = 0x4c,
	SystemModuleInformationEx = 0x4d,
	SystemVerifierTriageInformation = 0x4e,
	SystemSuperfetchInformation = 0x4f,
	SystemMemoryListInformation = 0x50,
	SystemFileCacheInformationEx = 0x51,
	SystemThreadPriorityClientIdInformation = 0x52,
	SystemProcessorIdleCycleTimeInformation = 0x53,
	SystemVerifierCancellationInformation = 0x54,
	SystemProcessorPowerInformationEx = 0x55,
	SystemRefTraceInformation = 0x56,
	SystemSpecialPoolInformation = 0x57,
	SystemProcessIdInformation = 0x58,
	SystemErrorPortInformation = 0x59,
	SystemBootEnvironmentInformation = 0x5a,
	SystemHypervisorInformation = 0x5b,
	SystemVerifierInformationEx = 0x5c,
	SystemTimeZoneInformation = 0x5d,
	SystemImageFileExecutionOptionsInformation = 0x5e,
	SystemCoverageInformation = 0x5f,
	SystemPrefetchPatchInformation = 0x60,
	SystemVerifierFaultsInformation = 0x61,
	SystemSystemPartitionInformation = 0x62,
	SystemSystemDiskInformation = 0x63,
	SystemProcessorPerformanceDistribution = 0x64,
	SystemNumaProximityNodeInformation = 0x65,
	SystemDynamicTimeZoneInformation = 0x66,
	SystemCodeIntegrityInformation = 0x67,
	SystemProcessorMicrocodeUpdateInformation = 0x68,
	SystemProcessorBrandString = 0x69,
	SystemVirtualAddressInformation = 0x6a,
	SystemLogicalProcessorAndGroupInformation = 0x6b,
	SystemProcessorCycleTimeInformation = 0x6c,
	SystemStoreInformation = 0x6d,
	SystemRegistryAppendString = 0x6e,
	SystemAitSamplingValue = 0x6f,
	SystemVhdBootInformation = 0x70,
	SystemCpuQuotaInformation = 0x71,
	SystemNativeBasicInformation = 0x72,
	SystemErrorPortTimeouts = 0x73,
	SystemLowPriorityIoInformation = 0x74,
	SystemBootEntropyInformation = 0x75,
	SystemVerifierCountersInformation = 0x76,
	SystemPagedPoolInformationEx = 0x77,
	SystemSystemPtesInformationEx = 0x78,
	SystemNodeDistanceInformation = 0x79,
	SystemAcpiAuditInformation = 0x7a,
	SystemBasicPerformanceInformation = 0x7b,
	SystemQueryPerformanceCounterInformation = 0x7c,
	SystemSessionBigPoolInformation = 0x7d,
	SystemBootGraphicsInformation = 0x7e,
	SystemScrubPhysicalMemoryInformation = 0x7f,
	SystemBadPageInformation = 0x80,
	SystemProcessorProfileControlArea = 0x81,
	SystemCombinePhysicalMemoryInformation = 0x82,
	SystemEntropyInterruptTimingInformation = 0x83,
	SystemConsoleInformation = 0x84,
	SystemPlatformBinaryInformation = 0x85,
	SystemThrottleNotificationInformation = 0x86,
	SystemHypervisorProcessorCountInformation = 0x87,
	SystemDeviceDataInformation = 0x88,
	SystemDeviceDataEnumerationInformation = 0x89,
	SystemMemoryTopologyInformation = 0x8a,
	SystemMemoryChannelInformation = 0x8b,
	SystemBootLogoInformation = 0x8c,
	SystemProcessorPerformanceInformationEx = 0x8d,
	SystemSpare0 = 0x8e,
	SystemSecureBootPolicyInformation = 0x8f,
	SystemPageFileInformationEx = 0x90,
	SystemSecureBootInformation = 0x91,
	SystemEntropyInterruptTimingRawInformation = 0x92,
	SystemPortableWorkspaceEfiLauncherInformation = 0x93,
	SystemFullProcessInformation = 0x94,
	SystemKernelDebuggerInformationEx = 0x95,
	SystemBootMetadataInformation = 0x96,
	SystemSoftRebootInformation = 0x97,
	SystemElamCertificateInformation = 0x98,
	SystemOfflineDumpConfigInformation = 0x99,
	SystemProcessorFeaturesInformation = 0x9a,
	SystemRegistryReconciliationInformation = 0x9b,
	MaxSystemInfoClass = 0x9c,
} SYSTEM_INFORMATION_CLASS;

typedef enum _eWinVersion {
	WINVER_7 = 0x0610,
	WINVER_7_SP1 = 0x0611,
	WINVER_8 = 0x0620,
	WINVER_81 = 0x0630,
	WINVER_10 = 0x0A00,
	WINVER_10_AU = 0x0A01,
} eWinVersion;

//处理进程信息，需要用到这两个结构体
typedef struct _SYSTEM_THREAD_INFORMATION {
    LARGE_INTEGER KernelTime;            // 0x0000 - 内核模式时间
    LARGE_INTEGER UserTime;              // 0x0008 - 用户模式时间
    LARGE_INTEGER CreateTime;            // 0x0010 - 线程创建时间
    ULONG WaitTime;                      // 0x0018 - 等待时间
    PVOID StartAddress;                  // 0x001C - 线程起始地址
    CLIENT_ID ClientId;                  // 0x0024 - 客户端ID (包含UniqueProcessId和UniqueThreadId)
    KPRIORITY Priority;                  // 0x0030 - 线程优先级
    LONG BasePriority;                   // 0x0032 - 基础优先级
    ULONG ContextSwitchCount;            // 0x0034 - 上下文切换计数
    ULONG State;                         // 0x0038 - 线程状态 (THREAD_STATE枚举)
    KWAIT_REASON WaitReason;             // 0x003C - 等待原因
} SYSTEM_THREAD_INFORMATION, *PSYSTEM_THREAD_INFORMATION;
 
//进程信息结构体  
typedef struct _SYSTEM_PROCESS_INFORMATION {
    ULONG NextEntryOffset;               // 0x0000 - 下一个条目偏移量 (0表示结束)
    ULONG NumberOfThreads;               // 0x0004 - 线程数量
    LARGE_INTEGER WorkingSetPrivateSize; // 0x0008 - 私有工作集大小
    ULONG HardFaultCount;                // 0x0010 - 硬错误计数
    ULONG NumberOfThreadsHighWatermark;  // 0x0014 - 线程数量峰值
    ULONGLONG CycleTime;                 // 0x0018 - 处理器周期时间
    LARGE_INTEGER CreateTime;            // 0x0020 - 进程创建时间
    LARGE_INTEGER UserTime;              // 0x0028 - 用户模式时间
    LARGE_INTEGER KernelTime;            // 0x0030 - 内核模式时间
    UNICODE_STRING ImageName;            // 0x0038 - 进程映像名称
    KPRIORITY BasePriority;              // 0x0048 - 基础优先级
    HANDLE UniqueProcessId;              // 0x004C - 进程唯一ID
    PVOID ParentProcessId;               // 0x0050 - 父进程ID
    ULONG ProcessQuotaUsage[2];          // 0x0058 - 进程配额使用情况
    ULONG ProcessQuotaPeak[2];           // 0x0060 - 进程配额峰值
    ULONG CommitCharge;                  // 0x0068 - 提交费用
    ULONG PeakVirtualSize;               // 0x006C - 虚拟内存峰值
    ULONG VirtualSize;                   // 0x0070 - 虚拟内存大小
    ULONG SessionId;                     // 0x0074 - 会话ID
    PVOID ImageBaseAddress;              // 0x0078 - 映像基址
    PVOID PeakWorkingSetSize;            // 0x0080 - 工作集峰值
    PVOID WorkingSetSize;                // 0x0088 - 工作集大小
    PVOID QuotaPeakPagedPoolUsage;       // 0x0090 - 分页池配额峰值
    PVOID QuotaPagedPoolUsage;           // 0x0098 - 分页池使用量
    PVOID QuotaPeakNonPagedPoolUsage;    // 0x00A0 - 非分页池配额峰值
    PVOID QuotaNonPagedPoolUsage;        // 0x00A8 - 非分页池使用量
    PVOID PagefileUsage;                 // 0x00B0 - 页面文件使用量
    PVOID PeakPagefileUsage;             // 0x00B8 - 页面文件使用峰值
    PVOID PrivatePageCount;              // 0x00C0 - 私有页面计数
    LARGE_INTEGER ReadOperationCount;    // 0x00C8 - 读操作计数
    LARGE_INTEGER WriteOperationCount;   // 0x00D0 - 写操作计数
    LARGE_INTEGER OtherOperationCount;   // 0x00D8 - 其他操作计数
    LARGE_INTEGER ReadTransferCount;     // 0x00E0 - 读传输计数
    LARGE_INTEGER WriteTransferCount;    // 0x00E8 - 写传输计数
    LARGE_INTEGER OtherTransferCount;    // 0x00F0 - 其他传输计数
    SYSTEM_THREAD_INFORMATION Threads[1];// 0x00F8 - 线程信息数组 (实际数量由NumberOfThreads指定)
} SYSTEM_PROCESS_INFORMATION, *PSYSTEM_PROCESS_INFORMATION;

typedef struct _LDR_DATA_TABLE_ENTRY_KERNEL {
    LIST_ENTRY InLoadOrderLinks;
    LIST_ENTRY InMemoryOrderLinks;
    LIST_ENTRY InInitializationOrderLinks;

    PVOID DllBase;
    PVOID EntryPoint;
    ULONG SizeOfImage;

    UNICODE_STRING FullDllName;
    UNICODE_STRING BaseDllName;

    ULONG Flags;
    USHORT LoadCount;
    USHORT TlsIndex;
    LIST_ENTRY HashLinks;

    PVOID SectionPointer;
    ULONG CheckSum;
    ULONG TimeDateStamp;

    PVOID LoadedImports;
    PVOID EntryPointActivationContext;
    PVOID PatchInformation;
    LIST_ENTRY ForwarderLinks;
    LIST_ENTRY ServiceTagLinks;
    LIST_ENTRY StaticLinks;
    PVOID ContextInformation;
    ULONG OriginalBase;
    LARGE_INTEGER LoadTime;
} LDR_DATA_TABLE_ENTRY_KERNEL, *PLDR_DATA_TABLE_ENTRY_KERNEL;

typedef struct _RTL_PROCESS_MODULE_INFORMATION {
	HANDLE Section;
	PVOID MappedBase;
	PVOID ImageBase;
	ULONG ImageSize;
	ULONG Flags;
	USHORT LoadOrderIndex;
	USHORT InitOrderIndex;
	USHORT LoadCount;
	USHORT __PADDING__;
	ULONG TimeDateStamp;
	CHAR FullPathName[256];
} RTL_PROCESS_MODULE_INFORMATION, *PRTL_PROCESS_MODULE_INFORMATION;

typedef struct _RTL_PROCESS_MODULES {
	ULONG NumberOfModules;
	RTL_PROCESS_MODULE_INFORMATION Modules[1];
} RTL_PROCESS_MODULES, *PRTL_PROCESS_MODULES;

typedef struct _SYSTEM_HANDLE_TABLE_ENTRY_INFO {
    USHORT UniqueProcessId;    // 持有句柄的进程PID
    USHORT CreatorBackTraceIndex;
    UCHAR ObjectTypeIndex;     // 对象类型索引
    UCHAR HandleAttributes;    // 句柄属性（如可继承）
    USHORT HandleValue;        // 句柄值（实际句柄=HandleValue）
    PVOID Object;              // 对象内核地址
    ULONG GrantedAccess;       // 访问权限掩码
} SYSTEM_HANDLE_TABLE_ENTRY_INFO, *PSYSTEM_HANDLE_TABLE_ENTRY_INFO;

typedef struct _SYSTEM_HANDLE_INFORMATION {
    ULONG NumberOfHandles;     // 句柄总数
    SYSTEM_HANDLE_TABLE_ENTRY_INFO Handles[1]; // 动态数组
} SYSTEM_HANDLE_INFORMATION, *PSYSTEM_HANDLE_INFORMATION;

typedef struct _LDR_DATA_TABLE_ENTRY
{
	LIST_ENTRY		InLoadOrderLinks;
	LIST_ENTRY		InMemoryOrderLinks;
	LIST_ENTRY		InInitializationOrderLinks;
	PVOID			DllBase;
	PVOID			EntryPoint;
	UINT32			SizeOfImage;
	UNICODE_STRING  FullDllName;
	UNICODE_STRING  BaseDllName;
	UINT32			Flags;
	UINT16			LoadCount;
	UINT16			TlsIndex;
	LIST_ENTRY		HashLinks;
	UINT32			TimeDateStamp;
} LDR_DATA_TABLE_ENTRY, *PLDR_DATA_TABLE_ENTRY;

typedef struct _PEB_LDR_DATA
{
	UINT32 Length;
	UINT8 Initialized;
	PVOID SsHandle;
	LIST_ENTRY InLoadOrderModuleList;
	LIST_ENTRY InMemoryOrderModuleList;
	LIST_ENTRY InInitializationOrderModuleList;
} PEB_LDR_DATA, *PPEB_LDR_DATA;

typedef struct _PEB
{
	UINT8 InheritedAddressSpace;
	UINT8 ReadImageFileExecOptions;
	UINT8 BeingDebugged;
	UINT8 BitField;
	PVOID Mutant;
	PVOID ImageBaseAddress;
	PPEB_LDR_DATA Ldr;
	PVOID ProcessParameters;
	PVOID SubSystemData;
	PVOID ProcessHeap;
	PVOID FastPebLock;
	PVOID AtlThunkSListPtr;
	PVOID IFEOKey;
	PVOID CrossProcessFlags;
	PVOID UserSharedInfoPtr;
	UINT32 SystemReserved;
	UINT32 AtlThunkSListPtr32;
	PVOID ApiSetMap;
} PEB, *PPEB;

typedef struct _OB_CALLBACK
{
	LIST_ENTRY ListEntry;
	ULONG64 Unknown;
	HANDLE ObHandle;
	PVOID ObTypeAddr;
	PVOID PreCall;
	PVOID PostCall;
} OB_CALLBACK, *POB_CALLBACK;
 
//声明ZqQueryAyatemInformation
typedef NTSTATUS (NTAPI *_ZwQuerySystemInformation) (
	IN ULONG SystemInformationClass,  //处理进程信息,只需要处理类别为5的即可
	OUT PVOID SystemInformation,
	IN ULONG SystemInformationLength,
	OUT PULONG ReturnLength
);
typedef NTSTATUS (__fastcall *_PspTerminateThreadByPointer)(
	PETHREAD pEThread,
	NTSTATUS ntExitCode,
	BOOLEAN bDirectTerminate
);
typedef BOOLEAN (*EX_ENUMERATE_HANDLE_ROUTINE)(
	PVOID HandleTableEntry, 
	HANDLE Handle, 
	PVOID EnumParameter
);
typedef NTSTATUS (NTAPI *_ExEnumHandleTable)(
	PVOID HandleTable, 
	EX_ENUMERATE_HANDLE_ROUTINE EnumProcedure, 
	PVOID EnumParameter, 
	PHANDLE HandleLast
);
typedef NTSTATUS (NTAPI *_NtSuspendProcess) (HANDLE ProcessHandle);
typedef NTSTATUS (NTAPI *_NtResumeProcess) (HANDLE ProcessHandle);


NTKERNELAPI PPEB PsGetProcessPeb(PEPROCESS Process);

_ZwQuerySystemInformation ZwQuerySystemInformation;
_PspTerminateThreadByPointer PspTerminateThreadByPointer;
_NtSuspendProcess NtSuspendProcess;
_NtResumeProcess NtResumeProcess;
_ExEnumHandleTable ExEnumHandleTable;

typedef struct _CALLBACK_CONTEXT{
	ULONG OpenProcessCallbackContextUsedCount;
	ULONG ProcessIDs[PROCESS_PROTECT_COUNT];
} CALLBACK_CONTEXT, *PCALLBACK_CONTEXT;

typedef struct _DEVICE_EXTENSION{
	SHORT OpenCount;
	HANDLE OpenProcessCallbackHandle;
	PCALLBACK_CONTEXT OpenProcessCallbackContext;
} DEVICE_EXTENSION, *PDEVICE_EXTENSION;

typedef struct _PROCESS_INFORMATION{
	//SIZE_T sizeOfInfo;
	ULONG64 dwPID;
	ULONG64 ParentPID;
	WCHAR imageName[64];
	//ULONG64 SessionID;
	ULONG64 EProcessAddr;
	//ULONG64 NumberOfThreads;
} PROCESS_INFORMATION, *PPROCESS_INFORMATION;

typedef struct _QUERY_PROCESS_INFORMATION{
	SIZE_T sizeOfInfo;
	USHORT numberOfProcess;
	PROCESS_INFORMATION ProcessInformation[1];
} QUERY_PROCESS_INFORMATION, *PQUERY_PROCESS_INFORMATION;

typedef struct _THREAD_INFORMATION{
	ULONG64 threadId;
	//ULONG64 TebAddr;
	ULONG64 ThreadEntry;
	LONG32 basePriority;
	ULONG64 State;
	
} THREAD_INFORMATION, *PTHREAD_INFORMATION;

typedef struct _QUERY_THREAD_INFORMATION{
	ULONG64 sizeOfInfo;
	ULONG64 numberOfThreads;
	THREAD_INFORMATION infos[1];
} QUERY_THREAD_INFORMATION, *PQUERY_THREAD_INFORMATION;

typedef struct _HANDLE_INFORMATION{
	UCHAR objectType;
	USHORT HandleValue;
	ULONG64 ObjectAddr;
	ULONG32 GrantedAccess;
} HANDLE_INFORMATION, *PHANDLE_INFORMATION;

typedef struct _QUERY_HANDLE_INFORMATION{
	ULONG64 sizeOfInfo;
	ULONG64 numberOfHandles;
	HANDLE_INFORMATION infos[1];
} QUERY_HANDLE_INFORMATION, *PQUERY_HANDLE_INFORMATION;

typedef struct _QUERY_SSDT_INFORMATION{
	ULONG64 KiServiceTableAddr;
	ULONG64 KernelBase;
	ULONG64 Addrs[1];
} QUERY_SSDT_INFORMATION, *PQUERY_SSDT_INFORMATION;

typedef struct _QUERY_SSSDT_INFORMATION{
	ULONG64 Win32kBase;
	ULONG64 w32pServiceTableAddr;
	ULONG64 Addrs[1];
} QUERY_SSSDT_INFORMATION, *PQUERY_SSSDT_INFORMATION;

typedef struct _QUERY_OBJECT_TYPE_INFORMATION{
	ULONG64 numberOfObjectTypes;
	WCHAR names[1][24];
} QUERY_OBJECT_TYPE_INFORMATION, *PQUERY_OBJECT_TYPE_INFORMATION;

typedef struct _CALLBACK_INFORMATION{
	ULONG64 bFromPsProcessType;
	ULONG64 PreCallback;
	ULONG64 PostCallback;
} CALLBACK_INFORMATION, *PCALLBACK_INFORMATION;

typedef struct _QUERY_OBJECT_TYPE_CALLBACK_INFORMATION{
	ULONG64 numberOfCallbacks;
	CALLBACK_INFORMATION infos[1];
} QUERY_OBJECT_TYPE_CALLBACK_INFORMATION, *PQUERY_OBJECT_TYPE_CALLBACK_INFORMATION;

typedef struct _SERVICE_DESCRIPTOR_TABLE {
    PULONG ServiceTableBase;
    PULONG ServiceCounterTableBase;
    ULONG NumberOfServices;
    PUCHAR ParamTableBase;
} SERVICE_DESCRIPTOR_TABLE, *PSERVICE_DESCRIPTOR_TABLE;

// HANDLE_TABLE* PspCidTable;
ULONG64 PspCidTable;
ULONG64 ObTypeIndexTable;

PSERVICE_DESCRIPTOR_TABLE AddrSSDT, AddrSSSDT;
PVOID NtOsKrnlBase, Win32kBase;

struct {
	UINT16 EPROCESS_UniqueProcessId;
	UINT16 EPROCESS_ActiveProcessLinks;
	
} DynamicData;

VOID DriverUnload(PDRIVER_OBJECT DriverObject);
NTSTATUS DispatchTerminateThread(PVOID inputBuffer, ULONG inputLength);
NTSTATUS DispatchQueryThreads(PVOID inputBuffer, ULONG inputLength, PVOID outputBuffer, ULONG outputLength, PIRP Irp);
NTSTATUS DispatchQueryProcess(PVOID outputBuffer, ULONG outputLength, PIRP Irp);
OB_PREOP_CALLBACK_STATUS PreOpenProcessCallback(PVOID RegistrationContext, POB_PRE_OPERATION_INFORMATION OperationInformation);
NTSTATUS DispatchSuccess(PDEVICE_OBJECT DeviceObject, PIRP Irp);
NTSTATUS DispatchTerminateProcess(PVOID inputBuffer, ULONG inputLength);
NTSTATUS ProtectProcess(PDEVICE_OBJECT DeviceObject);
NTSTATUS DispatchChangeProtectProcess(PVOID inputBuffer, ULONG inputLength, PDEVICE_OBJECT DeviceObject);
NTSTATUS DispatchDeviceControl(PDEVICE_OBJECT DeviceObject, PIRP Irp);
NTSTATUS DispatchClose(PDEVICE_OBJECT DeviceObject, PIRP Irp);
NTSTATUS DispatchCreate(PDEVICE_OBJECT DeviceObject, PIRP Irp);
NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath);
NTSTATUS DispatchQueryProcessHandle(PVOID inputBuffer, ULONG inputLength, PVOID outputBuffer, ULONG outputLength, PIRP Irp);
//NTSTATUS DispatchSuspendProcess(PVOID inputBuffer, ULONG inputLength);
//NTSTATUS DispatchResumeProcess(PVOID inputBuffer, ULONG inputLength);
NTSTATUS GetPspTerminateThreadByPointer();
NTSTATUS GetSSDTandSSSDT();
NTSTATUS GetSSDTandSSSDTFuncByIndex(BOOLEAN IsSSDTFunc, ULONG32 Index, PVOID* pFuncAddr);
NTSTATUS GetFunctionLengthMax(PVOID FuncAddr, PULONG32 pLength, ULONG32 MaxSearchSize);
