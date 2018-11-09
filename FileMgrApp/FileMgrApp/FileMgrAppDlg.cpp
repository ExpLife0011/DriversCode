
// FileMgrAppDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "FileMgrApp.h"
#include "FileMgrAppDlg.h"
#include "afxdialogex.h"
#include <winsvc.h>
#include <conio.h>
#include <winioctl.h>
#include <atlconv.h>

#define DRIVER_NAME L"FileDriver"
#define DRIVER_PATH L".\\FileDriver.sys"

#define IOCTRL_BASE 0x800
#define FILEIOCTRL_CODE(i) CTL_CODE(FILE_DEVICE_UNKNOWN, IOCTRL_BASE + i, METHOD_BUFFERED, FILE_ANY_ACCESS)

#define CTL_CREATEFILE FILEIOCTRL_CODE(0)
#define CTL_CREATEDIRECTORY FILEIOCTRL_CODE(1)
#define CTL_WRITEFILE FILEIOCTRL_CODE(2)
#define CTL_READFILE FILEIOCTRL_CODE(3)
#define CTL_COPYFILE FILEIOCTRL_CODE(4)
#define CTL_MOVEFILE FILEIOCTRL_CODE(5)
#define CTL_DELETEFILE FILEIOCTRL_CODE(6)
#define CTL_GETFILEATTRIBUTES FILEIOCTRL_CODE(7)
#define CTL_SETFILEATTRIBUTE FILEIOCTRL_CODE(8)
#define CTL_FORCEDELETEFILE FILEIOCTRL_CODE(9)




#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CFileMgrAppDlg �Ի���

//��������
BOOL LoadDriver(WCHAR *lpszDriverName, WCHAR *lpszDriverPath);
//ж������;
BOOL UnloadDriver(WCHAR *szSeverName);


CFileMgrAppDlg::CFileMgrAppDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CFileMgrAppDlg::IDD, pParent)
	, m_filePath(_T(""))
	, m_fileCMPath(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CFileMgrAppDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_ET_PATH, m_filePath);
	DDX_Text(pDX, IDC_ET_CMPATH, m_fileCMPath);
}

BEGIN_MESSAGE_MAP(CFileMgrAppDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_GETPATH, &CFileMgrAppDlg::OnBnClickedBtnGetpath)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BTN_CREATEFILE, &CFileMgrAppDlg::OnBnClickedBtnCreatefile)
	ON_BN_CLICKED(IDC_BTN_CREATEDIRECTORY, &CFileMgrAppDlg::OnBnClickedBtnCreatedirectory)
	ON_BN_CLICKED(IDC_BTN_DELETEFILE, &CFileMgrAppDlg::OnBnClickedBtnDeletefile)
	ON_BN_CLICKED(IDC_BTN_FORCEDELFILE, &CFileMgrAppDlg::OnBnClickedBtnForcedelfile)
	ON_BN_CLICKED(IDC_BTN_WRITEFILE, &CFileMgrAppDlg::OnBnClickedBtnWritefile)
	ON_BN_CLICKED(IDC_BTN_READFILE2, &CFileMgrAppDlg::OnBnClickedBtnReadfile2)
	ON_BN_CLICKED(IDC_BTN_COPYFILE, &CFileMgrAppDlg::OnBnClickedBtnCopyfile)
	ON_BN_CLICKED(IDC_BTN_MOVEFILE, &CFileMgrAppDlg::OnBnClickedBtnMovefile)
	ON_BN_CLICKED(IDC_BTN_GETCMPATH, &CFileMgrAppDlg::OnBnClickedBtnGetcmpath)
END_MESSAGE_MAP()


// CFileMgrAppDlg ��Ϣ�������

BOOL CFileMgrAppDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	ShowWindow(SW_MINIMIZE);

	// TODO:  �ڴ���Ӷ���ĳ�ʼ������
	//��������
	bRet = LoadDriver(DRIVER_NAME, DRIVER_PATH);
	if (!bRet)
	{
		MessageBox(L"��������ʧ�ܣ�", L"tips", MB_OK);
		//printf("LoadNtDriver Error!\n");
	}
	else
	{
		MessageBox(L"�������سɹ���", L"tips", MB_OK);
		//printf("LoadNtDriver ok;!\n");
	}

	//��������
	hDevice = CreateFile(L"\\\\.\\FileDriver", GENERIC_WRITE | GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
	if (hDevice != INVALID_HANDLE_VALUE)
	{
		//printf("Create Device ok;!\n");
		MessageBox(L"������������ɹ���", L"tips", MB_OK);
	}
	else
	{
		MessageBox(L"�����������ʧ�ܣ�", L"tips", MB_OK);
		//printf("Create Device Failed %d !\n", GetLastError());
	}

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CFileMgrAppDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CFileMgrAppDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CFileMgrAppDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CFileMgrAppDlg::OnBnClickedBtnGetpath()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CFileDialog ObjFile(TRUE, NULL, NULL, 0, L"�����ļ�|*.*|");
	if (IDOK == ObjFile.DoModal())
	{
		m_filePath = ObjFile.GetPathName();
		UpdateData(FALSE);
		if (m_filePath.IsEmpty())
		{
			MessageBox(L"·����ȡʧ��!", L"tips", MB_OK);
		}
	}
}

void CFileMgrAppDlg::OnClose()
{
	// TODO:  �ڴ������Ϣ�����������/�����Ĭ��ֵ
	//�ر��������
	CloseHandle(hDevice);
	bRet = UnloadDriver(DRIVER_NAME);
	if (!bRet)
	{
		//printf("UnloadNtDriver Error!\n");
		MessageBox(L"ж������ʧ�ܣ�", L"tips", MB_OK);
	}
	else
	{
		MessageBox(L"ж�������ɹ���", L"tips", MB_OK);
	}
	CDialogEx::OnClose();
}

void CFileMgrAppDlg::OnBnClickedBtnCreatefile()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	DriverFunction(CTL_CREATEFILE);
}


void CFileMgrAppDlg::OnBnClickedBtnCreatedirectory()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	DriverFunction(CTL_CREATEDIRECTORY);
}


void CFileMgrAppDlg::OnBnClickedBtnDeletefile()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	DriverFunction(CTL_DELETEFILE);
	
}


void CFileMgrAppDlg::OnBnClickedBtnForcedelfile()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	DriverFunction(CTL_FORCEDELETEFILE);
}


void CFileMgrAppDlg::OnBnClickedBtnWritefile()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	DriverFunction(CTL_WRITEFILE);
}


void CFileMgrAppDlg::OnBnClickedBtnReadfile2()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	DriverFunction(CTL_READFILE);
}

struct _PATH
{
	WCHAR bufFileSrcInput[128];
	WCHAR bufFileDstInput[128];
};
void CFileMgrAppDlg::OnBnClickedBtnCopyfile()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);
	_PATH path;
	_PATH out = {0};
	path.bufFileSrcInput[128] = { 0 };
	path.bufFileDstInput[128] = { 0 };
	for (int i = 0; i < m_filePath.GetLength(); i++)
	{
		path.bufFileSrcInput[i] = (WCHAR)m_filePath.GetAt(i);
	}
	
	for (int j = 0; j < m_fileCMPath.GetLength(); j++)
	{
		path.bufFileDstInput[j] = (WCHAR)m_fileCMPath.GetAt(j);
	}
	//WCHAR bufOutput[520] = { 0 };
	DWORD dwRet = 0;
	int n = sizeof(path);
	int m = sizeof(out);
	DeviceIoControl(hDevice, CTL_COPYFILE, &path, sizeof(path), &out, sizeof(out), &dwRet, NULL);
	if (dwRet)
	{
		MessageBox(L"����ִ�гɹ�!", L"tips", MB_OK);
	}
	else
	{
		MessageBox(L"����ִ��ʧ��!", L"tips", MB_OK);
	}
}


void CFileMgrAppDlg::OnBnClickedBtnMovefile()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);
	_PATH path = { 0 };
	_PATH out = { 0 };
	path.bufFileSrcInput[128] = { 0 };
	for (int i = 0; i < m_filePath.GetLength(); i++)
	{
		path.bufFileSrcInput[i] = (WCHAR)m_filePath.GetAt(i);
	}
	path.bufFileDstInput[128] = { 0 };
	for (int j = 0; j < m_fileCMPath.GetLength(); j++)
	{
		path.bufFileDstInput[j] = (WCHAR)m_fileCMPath.GetAt(j);
	}
	//WCHAR bufOutput[520] = { 0 };
	DWORD dwRet = 0;
	int n = sizeof(path);
	int m = sizeof(out);
	DeviceIoControl(hDevice, CTL_MOVEFILE, &path, sizeof(path), &out, sizeof(out), &dwRet, NULL);
	if (dwRet)
	{
		MessageBox(L"����ִ�гɹ�!", L"tips", MB_OK);
	}
	else
	{
		MessageBox(L"����ִ��ʧ��!", L"tips", MB_OK);
	}
}

void CFileMgrAppDlg::OnBnClickedBtnGetcmpath()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CFileDialog ObjFile(TRUE, NULL, NULL, 0, L"�����ļ�|*.*|");
	if (IDOK == ObjFile.DoModal())
	{
		m_fileCMPath = ObjFile.GetPathName();
		UpdateData(FALSE);
		if (m_filePath.IsEmpty())
		{
			MessageBox(L"·����ȡʧ��!", L"tips", MB_OK);
		}
	}
}

BOOL LoadDriver(WCHAR *lpszDriverName, WCHAR *lpszDriverPath)
{
	WCHAR szDriverImagePath[256] = { 0 };
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
BOOL UnloadDriver(WCHAR *szSeverName)
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
		printf("DeleteService() Fialed!\n");
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

VOID CFileMgrAppDlg::DriverFunction(int ioctrl)
{
	UpdateData(TRUE);
	WCHAR bufFileInput[512] = { 0 };
	for (int i = 0; i < m_filePath.GetLength(); i++)
	{
		bufFileInput[i] = m_filePath.GetAt(i);
	}
	WCHAR bufOutput[512] = { 0 };
	DWORD dwRet = 0;

	DeviceIoControl(hDevice, ioctrl, bufFileInput, 1024, bufOutput, 1024, &dwRet, NULL);
	if (dwRet)
	{
		MessageBox(bufOutput, L"tips", MB_OK);
	}
	else
	{
		MessageBox(L"����ִ��ʧ��!", L"tips", MB_OK);
	}
}