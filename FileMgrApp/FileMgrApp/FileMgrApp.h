
// FileMgrApp.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CFileMgrAppApp: 
// �йش����ʵ�֣������ FileMgrApp.cpp
//

class CFileMgrAppApp : public CWinApp
{
public:
	CFileMgrAppApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CFileMgrAppApp theApp;