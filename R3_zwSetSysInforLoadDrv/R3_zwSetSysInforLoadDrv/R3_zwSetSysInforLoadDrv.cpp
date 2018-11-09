// R3_zwSetSysInforLoadDrv.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <Windows.h>
#define NT_SUCCESS(Status) ((NTSTATUS)(Status) >= 0)
#define SystemLoadAndCallImage 38

typedef struct _UNICODE_STRING {
	USHORT Length;
	USHORT MaximumLength;
	PVOID Buffer;
}UNICODE_STRING, *PUNICODE_STRING;

typedef struct _SYSTEM_LOAD_AND_CALL_IMAGE{
	UNICODE_STRING ModuleName;
} SYSTEM_LOAD_AND_CALL_IMAGE, *PSYSTEM_LOAD_AND_CALL_IMAGE;

//��������������ԭ��
typedef DWORD(CALLBACK* ZWSETSYSTEMINFORMATION)(DWORD, PVOID, LONG);
ZWSETSYSTEMINFORMATION ZwSetSystemInformation;
typedef DWORD (CALLBACK* RTLINITUNICODESTRING)(PUNICODE_STRING, PCWSTR);
RTLINITUNICODESTRING RtlInitUnicodeString;
typedef DWORD(CALLBACK* RTLANSISTRINGTOUNICODESTRING)(PVOID, PVOID, DWORD);
RTLANSISTRINGTOUNICODESTRING RtlAnsiStringToUnicodeString;

int _tmain(int argc, _TCHAR* argv[])
{
	SYSTEM_LOAD_AND_CALL_IMAGE gregsImage;
	UNICODE_STRING TmpBuff;
	char szDrvFullPath[256];
	int iBufflen;
	//�ֹ���������������
	if  (!(RtlInitUnicodeString = (RTLINITUNICODESTRING)GetProcAddress(GetModuleHandle(L"ntdll.dll"), "RtlInitUnicodeString")))
	{
		printf("GetProcAddrss error:%d\n", GetLastError());
		exit(-1);
	}
	if (!(RtlAnsiStringToUnicodeString = (RTLANSISTRINGTOUNICODESTRING)GetProcAddress(GetModuleHandle(L"ntdll.dll"), "RtlAnsiStringToUnicodeString")))
	{
		printf("GetProcAddrss error:%d\n", GetLastError());
		exit(-1);
	}
	if (!(ZwSetSystemInformation = (ZWSETSYSTEMINFORMATION)GetProcAddress(GetModuleHandle(L"ntdll.dll"), "ZwSetSystemInformation")))
	{
		printf("GetProcAddrss error:%d\n", GetLastError());
		exit(-1);
	}
	//ring 0 ���ʴ���ʱҪ��\\??\\%s �ķ�ʽ, ��ǰ����ʱ����һ��?
	iBufflen = sprintf(szDrvFullPath, "\\??\\%s", "c:\\MyDriver1.sys");
	szDrvFullPath[iBufflen] = 0;
	TmpBuff.Buffer = (PVOID)szDrvFullPath;
	TmpBuff.Length = iBufflen;
	RtlAnsiStringToUnicodeString(&(gregsImage.ModuleName), &TmpBuff, 1);
	if (NT_SUCCESS(ZwSetSystemInformation(SystemLoadAndCallImage, &gregsImage,
		sizeof(SYSTEM_LOAD_AND_CALL_IMAGE))))
	{
		printf("drive %s was loaded....", szDrvFullPath);
	}
	else
	{
		printf("dirve load error...");
	}
	getchar();
	return 0;
}

