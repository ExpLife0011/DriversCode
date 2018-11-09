
// FileMgrAppDlg.h : ͷ�ļ�
//

#pragma once


// CFileMgrAppDlg �Ի���
class CFileMgrAppDlg : public CDialogEx
{
// ����
public:
	CFileMgrAppDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_FILEMGRAPP_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;
	
	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnGetpath();
	CString m_filePath;
	afx_msg void OnClose();


public:
	BOOL bRet;
	HANDLE hDevice;
	VOID DriverFunction(int ioctrl);
	afx_msg void OnBnClickedBtnCreatefile();
	afx_msg void OnBnClickedBtnCreatedirectory();
	afx_msg void OnBnClickedBtnDeletefile();
	afx_msg void OnBnClickedBtnForcedelfile();
	afx_msg void OnBnClickedBtnWritefile();
	afx_msg void OnBnClickedBtnReadfile2();
	afx_msg void OnBnClickedBtnCopyfile();
	afx_msg void OnBnClickedBtnMovefile();
	CString m_fileCMPath;
	afx_msg void OnBnClickedBtnGetcmpath();
};
