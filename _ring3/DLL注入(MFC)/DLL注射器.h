
// DLLע����.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CDLLע����App: 
// �йش����ʵ�֣������ DLLע����.cpp
//

class CDLLע����App : public CWinApp
{
public:
	CDLLע����App();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CDLLע����App theApp;