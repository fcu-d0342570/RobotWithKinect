#pragma once
#include "ocx1.h"
#include <wmp.h>
#include "CWMPPlayer4.h"



// Player ��ܤ��

class Player : public CDialogEx
{
	DECLARE_DYNAMIC(Player)

public:
	Player(CWnd* pParent = NULL);   // �зǫغc�禡
	virtual ~Player();

// ��ܤ�����
	enum { IDD = IDD_PLAYER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �䴩

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedOk();
	CComPtr<IWMPPlayer> m_pWMPPlayer;
	//COcx1 m_player;
	CWMPPlayer4 m_Player;
};
