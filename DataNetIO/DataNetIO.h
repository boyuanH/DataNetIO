
// DataNetIO.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CDataNetIOApp:
// �йش����ʵ�֣������ DataNetIO.cpp
//

class CDataNetIOApp : public CWinApp
{
public:
	CDataNetIOApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CDataNetIOApp theApp;