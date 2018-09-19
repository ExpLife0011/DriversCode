// InstallNtDriver.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <windows.h>
#include <winsvc.h>
#include <conio.h>
#include <winioctl.h>
#define DRIVER_NAME "NtDriver"
#define DRIVER_PATH ".\\NtDriver.sys"

#define IOCTRL_BASE 0x800
#define MYIOCTRL_CODE(i) CTL_CODE(FILE_DEVICE_UNKNOWN, IOCTRL_BASE+i, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define CTL_HELLO MYIOCTRL_CODE(0)
#define CTL_PRINT MYIOCTRL_CODE(1)
#define CTL_BYE MYIOCTRL_CODE(2)

BOOL LoadDriver(char *lpszDriverName, char *lpszDriverPath)
{
	char szDriverImagePath[256] = { 0 };
	GetFullPathName(lpszDriverPath, 256, szDriverImagePath, NULL);

	BOOL bRet = FALSE;

	SC_HANDLE hServiceMgr = NULL;
	SC_HANDLE hServiceDDK = NULL;

	//�򿪷�����ƹ�����;
	hServiceMgr = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

	if (hServiceMgr == NULL)
	{
		printf("OpenSCManager() Failed %d!\n", GetLastError());
		bRet = FALSE;
		goto BeforeLeave;
	}
	else
	{
		//OpenSCManager�ɹ�
		printf("OpenSCManager() ok!\n");
	}

	//����������Ӧ�ķ���;
	hServiceDDK = CreateService(hServiceMgr, lpszDriverName, lpszDriverName, 
		SERVICE_ALL_ACCESS, SERVICE_KERNEL_DRIVER, SERVICE_DEMAND_START,
		SERVICE_ERROR_IGNORE, szDriverImagePath, NULL, NULL, NULL, NULL, NULL);

	DWORD dwRtn;
	//�жϷ����Ƿ�ʧ��;
	if (hServiceDDK == NULL)
	{
		dwRtn = GetLastError();
		if (dwRtn != ERROR_IO_PENDING && dwRtn != ERROR_SERVICE_EXISTS)
		{
			//����ԭ��ʧ��;
			printf("CreateService() Failed %d !\n", dwRtn);
			bRet = FALSE;
			goto BeforeLeave;
		}
		else
		{
			//�����������;
			printf("CreateService() Failed Service is ERROR_IO_PENDING or ERROR_SERVICE_EXISTS!\n");
		}

		//�����Ѿ�����;
		hServiceDDK = OpenService(hServiceMgr, lpszDriverName, SERVICE_ALL_ACCESS);
		if (hServiceDDK == NULL)
		{
			dwRtn = GetLastError();
			printf("OpenService() Failed %d!\n", dwRtn);
			bRet = FALSE;
			goto BeforeLeave;
		}
		else
		{
			printf("OpenService() ok!\n");
		}
	}
	else
	{
		printf("CreateService() ok!\n");
	}

	//��������;
	bRet = StartService(hServiceDDK, NULL, NULL);
	if (!bRet)
	{
		DWORD dwRtn = GetLastError();
		if (dwRtn != ERROR_IO_PENDING && dwRtn != ERROR_SERVICE_ALREADY_RUNNING)
		{
			printf("StartService() Failed %d !\n", dwRtn);
			bRet = FALSE;
			goto BeforeLeave;
		}
		else
		{
			if (dwRtn == ERROR_IO_PENDING)
			{
				printf("StartService() Failed ERROR_IO_PENDING!\n");
				bRet = FALSE;
				goto BeforeLeave;
			}
			else
			{
				printf("StartService() Failed ERROR_SERVICE_ALREADY_RUNNING!\n");
				bRet = TRUE;
				goto BeforeLeave;
			}
		}
	}
	bRet = TRUE;
BeforeLeave:
	if (hServiceDDK)
	{
		CloseServiceHandle(hServiceDDK);
	}
	if (hServiceMgr)
	{
		CloseServiceHandle(hServiceMgr);
	}
	return bRet;
}

//ж������;
BOOL UnloadDriver(char *szSeverName)
{
	BOOL bRet = FALSE;
	SC_HANDLE hServiceMgr = NULL;
	SC_HANDLE hServiceDDK = NULL;
	SERVICE_STATUS ServiceStatus;

	hServiceMgr = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (hServiceMgr == NULL)
	{
		printf("OpenScManager() Fialed %d!\n", GetLastError());
		bRet = FALSE;
		goto BeforeLeave;
	}
	else
	{
		printf("OpenSCManager() ok !\n");
	}

	hServiceDDK = OpenService(hServiceMgr, szSeverName, SERVICE_ALL_ACCESS);
	if (hServiceDDK == NULL)
	{
		printf("OpenService() Failed %d !\n", GetLastError());
		bRet = FALSE;
		goto BeforeLeave;
	}
	else
	{
		printf("OpenService() ok!\n");
	}
	//ֹͣ�����������ʧ�ܣ�ֻ�������������ܣ��ٶ�̬����;
	if (!ControlService(hServiceDDK, SERVICE_CONTROL_STOP, &ServiceStatus))
	{
		printf("ControlService() Fialed %d !\n", GetLastError());
	}
	else
	{
		printf("ControlService() ok!\n");
	}

	//��̬ж������;
	if (!DeleteService(hServiceDDK))
	{
		printf("DeleteService() Fialed %d !\n");
	}
	else
	{
		printf("DeleteService() ok!\n");
	}
	bRet = TRUE;

BeforeLeave:
	if (hServiceDDK)
	{
		CloseServiceHandle(hServiceDDK);
	}
	if (hServiceMgr)
	{
		CloseServiceHandle(hServiceMgr);
	}
	return bRet;
}

void TestDriver()
{
	HANDLE hDevice = CreateFile("\\\\.\\NtDriver", GENERIC_WRITE | GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
	if (hDevice != INVALID_HANDLE_VALUE)
	{
		printf("Create Device ok;!\n");
	}
	else
	{
		printf("Create Device Failed %d !\n", GetLastError());
		return;
	}

	CHAR bufRead[1024] = { 0 };
	WCHAR bufWrite[1024] = L"Hello world";

	DWORD dwRead = 0;
	DWORD dwWrite = 0;

	ReadFile(hDevice, bufRead, 1024, &dwRead, NULL);
	printf("Read done: %ws\n", bufRead);
	printf("please press any key to write\n");
	getch();
	WriteFile(hDevice, bufWrite, (wcslen(bufWrite) + 1) * sizeof(WCHAR), &dwWrite, NULL);

	printf("Write done!\n");

	printf("please press any key to deviceiocontrol;\n");
	getch();
	CHAR bufInput[1024] = "Hello world";
	CHAR bufOutput[1024] = { 0 };
	DWORD dwRet = 0;

	WCHAR bufFileInput[1024] = L"D:\\ntdriver.log";

	printf("please press any key to send PRINT\n");
	getch();
	DeviceIoControl(hDevice, CTL_PRINT, bufFileInput, sizeof(bufFileInput), bufOutput, sizeof(bufOutput), &dwRet, NULL);


	printf("please press any key to send HELLO\n");
	getch();
	DeviceIoControl(hDevice, CTL_HELLO, NULL, 0, NULL, 0, &dwRet, NULL);


	printf("please press any key to send BYE\n");
	getch();
	DeviceIoControl(hDevice, CTL_BYE, NULL, 0, NULL, 0, &dwRet, NULL);

	printf("DeviceIoControl done!\n");
	CloseHandle(hDevice);
}

int _tmain(int argc, _TCHAR* argv[])
{
	//��������;
	BOOL bRet = LoadDriver(DRIVER_NAME, DRIVER_PATH);
	if (!bRet)
	{
		printf("LoadNtDriver Error!\n");
		return 0;
	}
	else
	{
		printf("LoadNtDriver ok;!\n");
	}
	printf("press any key to create device!\n");
	getch();

	//��������;
	TestDriver();

	printf("press any key to stop service!\n");
	getch();

	//ж������;
	bRet = UnloadDriver(DRIVER_NAME);
	if (!bRet)
	{
		printf("UnloadNtDriver Error!\n");
		return 0;
	}

	return 0;
}

