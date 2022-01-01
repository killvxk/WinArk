#pragma once

/*
DeviceType - identifies a type of device. 
This can be one of the FILE_DEVICE_xxx constants defined in the WDK headers, 
but this is mostly for hardware based drivers. 
For software drivers like ours, the number doesn't matter much. 
Still, Microsoft's documentation specifies that values for 3rd parties should start with0x8000.
*/

/*
Function - an ascending number indicating a specific operation. 
If nothing else, this number must be different between different control codes for the same driver. 
Again, any number will do, but the official documentation says 3rd party drivers should start with 0x800.
*/

/*
Method - the most important part of the control code. 
It indicates how the input and output buffers provided by the client pass to the driver.
*/

/*
Access - indicates whether this operation is to the driver (FILE_WRITE_ACCESS), from the driver (FILE_READ_ACCESS), or both ways (FILE_ANY_ACCESS). 
Typical drivers just use FILE_ANY_ACCESS and deal with the actual request in the IRP_MJ_DEVICE_CONTROL handler.
*/

// 64bit��32bit�ں˿�������
// һ����������
//		1.Wow64��ϵͳ
//			�ļ��ض��� ���³���
//				%windir%\System32\catroot
//				%windir%\System32\catroot2
//				%windir%\System32\drivers\etc
//				%windir%\System32\logfiles
//				%windir%\System32\spool
//			ע����ض���
//		2.PatchGuard����
// ������̲���
//		1.��Ƕ���
//		2.Ԥ��������������
//		3.���ݽṹ����
//			������ݽṹ�����ָ�룬thunking�������ת����Ҫʹ�ù̶����ȵ��������������

// �ص����� ������ ������ + -----> /integritycheck
#define ANTI_ROOTKIT_DEVICE 0x8000

#define DRIVER_CURRENT_VERSION 0x2C

// ��MDL�����û��ڴ�
// METHOD_OUT_DIRECT in: Irp->AssociatedIrp.SystemBuffer out: Irp->MdlAddress write
// METHOD_IN_DIRECT in: Irp->AssociatedIrp.SystemBuffer out: Irp->MdlAddress read
	
#define IOCTL_ARK_OPEN_OBJECT						CTL_CODE(ANTI_ROOTKIT_DEVICE,0x800,METHOD_BUFFERED,FILE_ANY_ACCESS)
// ���巽ʽ
#define IOCTL_ARK_OPEN_PROCESS						CTL_CODE(ANTI_ROOTKIT_DEVICE,0x801,METHOD_BUFFERED,FILE_ANY_ACCESS)
#define IOCTL_ARK_GET_VERSION						CTL_CODE(ANTI_ROOTKIT_DEVICE,0x802,METHOD_BUFFERED,FILE_ANY_ACCESS)
#define IOCTL_ARK_DUP_HANDLE						CTL_CODE(ANTI_ROOTKIT_DEVICE,0x803,METHOD_BUFFERED,FILE_ANY_ACCESS)
#define IOCTL_ARK_OPEN_THREAD						CTL_CODE(ANTI_ROOTKIT_DEVICE,0x804,METHOD_BUFFERED,FILE_ANY_ACCESS)
#define IOCTL_ARK_OPEN_KEY							CTL_CODE(ANTI_ROOTKIT_DEVICE,0x805,METHOD_BUFFERED,FILE_ANY_ACCESS)
#define IOCTL_ARK_GET_SERVICE_TABLE					CTL_CODE(ANTI_ROOTKIT_DEVICE,0x806,METHOD_BUFFERED,FILE_ANY_ACCESS)
#define IOCTL_ARK_GET_SSDT_API_ADDR					CTL_CODE(ANTI_ROOTKIT_DEVICE,0x807,METHOD_BUFFERED,FILE_ANY_ACCESS)
#define IOCTL_ARK_GET_SERVICE_TABLE_OFFSET			CTL_CODE(ANTI_ROOTKIT_DEVICE,0x808,METHOD_BUFFERED,FILE_ANY_ACCESS)
#define IOCTL_ARK_GET_SHADOW_SSDT_API_ADDR			CTL_CODE(ANTI_ROOTKIT_DEVICE,0x809,METHOD_BUFFERED,FILE_ANY_ACCESS)
#define IOCTL_ARK_GET_SHADOW_SERVICE_LIMIT			CTL_CODE(ANTI_ROOTKIT_DEVICE,0x80A,METHOD_BUFFERED,FILE_ANY_ACCESS)
#define IOCTL_ARK_ENUM_PROCESS_NOTIFY				CTL_CODE(ANTI_ROOTKIT_DEVICE,0x80B,METHOD_BUFFERED,FILE_ANY_ACCESS)
#define IOCTL_ARK_GET_PROCESS_NOTIFY_COUNT			CTL_CODE(ANTI_ROOTKIT_DEVICE,0x80C,METHOD_BUFFERED,FILE_ANY_ACCESS)
#define IOCTL_ARK_GET_THREAD_NOTIFY_COUNT			CTL_CODE(ANTI_ROOTKIT_DEVICE,0x80D,METHOD_BUFFERED,FILE_ANY_ACCESS)
#define IOCTL_ARK_ENUM_THREAD_NOTIFY				CTL_CODE(ANTI_ROOTKIT_DEVICE,0x80E,METHOD_BUFFERED,FILE_ANY_ACCESS)
#define IOCTL_ARK_GET_IMAGE_NOTIFY_COUNT			CTL_CODE(ANTI_ROOTKIT_DEVICE,0x80F,METHOD_BUFFERED,FILE_ANY_ACCESS)
#define IOCTL_ARK_ENUM_IMAGELOAD_NOTIFY				CTL_CODE(ANTI_ROOTKIT_DEVICE,0x810,METHOD_BUFFERED,FILE_ANY_ACCESS)
#define IOCTL_ARK_ENUM_PIDDBCACHE_TABLE				CTL_CODE(ANTI_ROOTKIT_DEVICE,0x811,METHOD_NEITHER,FILE_ANY_ACCESS)
#define IOCTL_ARK_ENUM_UNLOADED_DRIVERS				CTL_CODE(ANTI_ROOTKIT_DEVICE,0x812,METHOD_NEITHER,FILE_ANY_ACCESS)
#define IOCTL_ARK_GET_UNLOADED_DRIVERS_COUNT		CTL_CODE(ANTI_ROOTKIT_DEVICE,0x813,METHOD_BUFFERED,FILE_ANY_ACCESS)
#define IOCTL_ARK_GET_PIDDBCACHE_DATA_SIZE			CTL_CODE(ANTI_ROOTKIT_DEVICE,0x814,METHOD_BUFFERED,FILE_ANY_ACCESS)
#define IOCTL_ARK_GET_UNLOADED_DRIVERS_DATA_SIZE	CTL_CODE(ANTI_ROOTKIT_DEVICE,0x815,METHOD_BUFFERED,FILE_ANY_ACCESS)

// ԭʼ��ʽ
// METHOD_NEITHER in: DeviceIoControl.Type3InputBuffer out: Irp->UersBuffer
#define IOCTL_ARK_SET_PRIORITY		CTL_CODE(ANTI_ROOTKIT_DEVICE,0x803,METHOD_NEITHER,FILE_ANY_ACCESS)

struct OpenObjectData {
	void* Address;
	ACCESS_MASK Access;
};

struct OpenProcessThreadData {
	ULONG Id;
	ACCESS_MASK AccessMask;
};

struct ThreadData {
	ULONG ThreadId;
	int Priority;
};

struct DupHandleData {
	ULONG Handle;
	ULONG SourcePid;
	ACCESS_MASK AccessMask;
	ULONG Flags;
};

struct KeyData {
	ULONG Length;
	ULONG Access;
	WCHAR Name[1];
};

struct ProcessNotifyCountData {
	PULONG pCount;
	PULONG pExCount;
};

struct NotifyInfo {
	void* pRoutine;
	ULONG Count;
};

struct KernelCallbackInfo {
	ULONG Count;
	void* Address[ANYSIZE_ARRAY];
};

struct ThreadNotifyCountData {
	PULONG pCount;
	PULONG pNonSystemCount;
};

struct UnloadedDriversInfo {
	void* pMmUnloadedDrivers;
	ULONG Count;
};

struct NameInfo {
	ULONG Offset;
	ULONG Length;
};

// I recommend you define a structure for user-mode that looks something like this:
struct PiDDBCacheData {
	USHORT NextEntryOffset;
	USHORT StringLen;
	USHORT StringOffset;
	ULONG TimeDateStamp;
	NTSTATUS LoadStatus;
};
// and the string characters would follow the structure in memory
// (the length and offset would point to where the string starts / ends)

struct UnloadedDriverData {
	USHORT NextEntryOffset;
	USHORT StringLen;
	USHORT StringOffset;
	PVOID StartAddress;
	PVOID EndAddress;
	LARGE_INTEGER CurrentTime;
};