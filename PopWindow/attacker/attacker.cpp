// attacker.cpp : Defines the entry point for the console application.
//
#include "StdAfx.h"
#include <windows.h>  
#include <winsvc.h>  
#include <conio.h>  
#include <stdio.h>
#include <winioctl.h>

#define DRIVER_NAME "PopupDrv"
#define DRIVER_PATH ".\\PopupDrv.sys"

#define IOCTL_BASE	0x800
#define MY_CTL_CODE(i) \
CTL_CODE(FILE_DEVICE_UNKNOWN, IOCTL_BASE+i, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define IOCTL_XXX_ATTACK	MY_CTL_CODE(2)

//װ��NT��������
BOOL LoadDriver(char* lpszDriverName,char* lpszDriverPath)
{
	char szDriverImagePath[256] = {0};
	//�õ�����������·��
	GetFullPathName(lpszDriverPath, 256, szDriverImagePath, NULL);

	BOOL bRet = FALSE;

	SC_HANDLE hServiceMgr=NULL;//SCM�������ľ��
	SC_HANDLE hServiceDDK=NULL;//NT��������ķ�����

	//�򿪷�����ƹ�����
	hServiceMgr = OpenSCManager( NULL, NULL, SC_MANAGER_ALL_ACCESS );

	if( hServiceMgr == NULL )  
	{
		//OpenSCManagerʧ��
		printf( "OpenSCManager() Failed %d ! \n", GetLastError() );
		bRet = FALSE;
		goto BeforeLeave;
	}
	else
	{
		////OpenSCManager�ɹ�
		printf( "OpenSCManager() ok ! \n" );  
	}

	//������������Ӧ�ķ���
	hServiceDDK = CreateService( hServiceMgr,
		lpszDriverName, //�����������ע����е�����  
		lpszDriverName, // ע������������ DisplayName ֵ  
		SERVICE_ALL_ACCESS, // ������������ķ���Ȩ��  
		SERVICE_KERNEL_DRIVER,// ��ʾ���صķ�������������  
		SERVICE_DEMAND_START, // ע������������ Start ֵ  
		SERVICE_ERROR_IGNORE, // ע������������ ErrorControl ֵ  
		szDriverImagePath, // ע������������ ImagePath ֵ  
		NULL,  //GroupOrder HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\GroupOrderList
		NULL,  
		NULL,  
		NULL,  
		NULL);  

	DWORD dwRtn;
	//�жϷ����Ƿ�ʧ��
	if( hServiceDDK == NULL )  
	{  
		dwRtn = GetLastError();
		if( dwRtn != ERROR_IO_PENDING && dwRtn != ERROR_SERVICE_EXISTS )  
		{  
			//��������ԭ�򴴽�����ʧ��
			printf( "CrateService() Failed %d ! \n", dwRtn );  
			bRet = FALSE;
			goto BeforeLeave;
		}  
		else  
		{
			//���񴴽�ʧ�ܣ������ڷ����Ѿ�������
			printf( "CrateService() Faild Service is ERROR_IO_PENDING or ERROR_SERVICE_EXISTS! \n" );  
		}

		// ���������Ѿ����أ�ֻ��Ҫ��  
		hServiceDDK = OpenService( hServiceMgr, lpszDriverName, SERVICE_ALL_ACCESS );  
		if( hServiceDDK == NULL )  
		{
			//����򿪷���Ҳʧ�ܣ�����ζ����
			dwRtn = GetLastError();  
			printf( "OpenService() Failed %d ! \n", dwRtn );  
			bRet = FALSE;
			goto BeforeLeave;
		}  
		else 
		{
			printf( "OpenService() ok ! \n" );
		}
	}  
	else  
	{
		printf( "CrateService() ok ! \n" );
	}

	//�����������
	bRet= StartService( hServiceDDK, NULL, NULL );  
	if( !bRet )  
	{  
		DWORD dwRtn = GetLastError();  
		if( dwRtn != ERROR_IO_PENDING && dwRtn != ERROR_SERVICE_ALREADY_RUNNING )  
		{  
			printf( "StartService() Failed %d ! \n", dwRtn );  
			bRet = FALSE;
			goto BeforeLeave;
		}  
		else  
		{  
			if( dwRtn == ERROR_IO_PENDING )  
			{  
				//�豸����ס
				printf( "StartService() Failed ERROR_IO_PENDING ! \n");
				bRet = FALSE;
				goto BeforeLeave;
			}  
			else  
			{  
				//�����Ѿ�����
				printf( "StartService() Failed ERROR_SERVICE_ALREADY_RUNNING ! \n");
				bRet = TRUE;
				goto BeforeLeave;
			}  
		}  
	}
	bRet = TRUE;
//�뿪ǰ�رվ��
BeforeLeave:
	if(hServiceDDK)
	{
		CloseServiceHandle(hServiceDDK);
	}
	if(hServiceMgr)
	{
		CloseServiceHandle(hServiceMgr);
	}
	return bRet;
}

//ж����������  
BOOL UnloadDriver( char * szSvrName )  
{
	BOOL bRet = FALSE;
	SC_HANDLE hServiceMgr=NULL;//SCM�������ľ��
	SC_HANDLE hServiceDDK=NULL;//NT��������ķ�����
	SERVICE_STATUS SvrSta;
	//��SCM������
	hServiceMgr = OpenSCManager( NULL, NULL, SC_MANAGER_ALL_ACCESS );  
	if( hServiceMgr == NULL )  
	{
		//����SCM������ʧ��
		printf( "OpenSCManager() Failed %d ! \n", GetLastError() );  
		bRet = FALSE;
		goto BeforeLeave;
	}  
	else  
	{
		//����SCM������ʧ�ܳɹ�
		printf( "OpenSCManager() ok ! \n" );  
	}
	//����������Ӧ�ķ���
	hServiceDDK = OpenService( hServiceMgr, szSvrName, SERVICE_ALL_ACCESS );  

	if( hServiceDDK == NULL )  
	{
		//����������Ӧ�ķ���ʧ��
		printf( "OpenService() Failed %d ! \n", GetLastError() );  
		bRet = FALSE;
		goto BeforeLeave;
	}  
	else  
	{  
		printf( "OpenService() ok ! \n" );  
	}  
	//ֹͣ�����������ֹͣʧ�ܣ�ֻ�������������ܣ��ٶ�̬���ء�  
	if( !ControlService( hServiceDDK, SERVICE_CONTROL_STOP , &SvrSta ) )  
	{  
		printf( "ControlService() Failed %d !\n", GetLastError() );  
	}  
	else  
	{
		//����������Ӧ��ʧ��
		printf( "ControlService() ok !\n" );  
	}  
	//��̬ж����������  
	if( !DeleteService( hServiceDDK ) )  
	{
		//ж��ʧ��
		printf( "DeleteSrevice() Failed %d !\n", GetLastError() );  
	}  
	else  
	{  
		//ж�سɹ�
		printf( "DelServer:DeleteSrevice() ok !\n" );  
	}  
	bRet = TRUE;
BeforeLeave:
//�뿪ǰ�رմ򿪵ľ��
	if(hServiceDDK)
	{
		CloseServiceHandle(hServiceDDK);
	}
	if(hServiceMgr)
	{
		CloseServiceHandle(hServiceMgr);
	}
	return bRet;	
} 

void TestDriver()
{
	//������������  
	HANDLE hDevice = CreateFile("\\\\.\\PopupDrv",  
		GENERIC_WRITE | GENERIC_READ,  
		0,  
		NULL,  
		OPEN_EXISTING,  
		0,  
		NULL);  
	if( hDevice != INVALID_HANDLE_VALUE )  
	{
		printf( "Create Device ok ! \n" );  
	}
	else  
	{
		printf( "Create Device failed %d ! \n", GetLastError() ); 
		return;
	}


	CHAR bufInput[1024] ="Hello, world";
	DWORD dwAttackRes = 0;
	DWORD dwRet = 0;
	printf("Begin to attack\n");

	DeviceIoControl(hDevice, IOCTL_XXX_ATTACK, bufInput, sizeof(bufInput), &dwAttackRes, sizeof(dwAttackRes), &dwRet, NULL);
	if (dwAttackRes == 0)
	{
		printf("Attack denied\n");
	}
	else
	{
		printf("Attack OK\n");
	}


	CloseHandle( hDevice );
} 

int main(int argc, char* argv[])  
{
	//��������
	BOOL bRet = LoadDriver(DRIVER_NAME,DRIVER_PATH);
	if (!bRet)
	{
		printf("LoadNTDriver error\n");
		return 0;
	}
	//���سɹ�

	//printf( "press any to create device!\n" );  
	//getch();  

	TestDriver();

	//��ʱ�������ͨ��ע������������鿴�������ӵ�������֤��  
	//printf( "press any to unload the driver!\n" );  
	//getch();  

	//ж������
// 	bRet = UnloadDriver(DRIVER_NAME);
// 	if (!bRet)
// 	{
// 		printf("UnloadNTDriver error\n");
// 		return 0;
// 	}

	return 0;  
}