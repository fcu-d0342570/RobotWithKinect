
// MFC_TEST.h : PROJECT_NAME ���ε{�����D�n���Y��
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�� PCH �]�t���ɮ׫e���]�t 'stdafx.h'"
#endif

#include "resource.h"		// �D�n�Ÿ�


// CMFC_TESTApp: 
// �аѾ\��@�����O�� MFC_TEST.cpp
//

class CMFC_TESTApp : public CWinApp
{
public:
	CMFC_TESTApp();

// �мg
public:
	virtual BOOL InitInstance();

// �{���X��@

	DECLARE_MESSAGE_MAP()
};

extern CMFC_TESTApp theApp;