#pragma once
#include "ocx1.h"
#include <wmp.h>
#include "CWMPPlayer4.h"



// Player 對話方塊

class Player : public CDialogEx
{
	DECLARE_DYNAMIC(Player)

public:
	Player(CWnd* pParent = NULL);   // 標準建構函式
	virtual ~Player();

// 對話方塊資料
	enum { IDD = IDD_PLAYER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedOk();
	CComPtr<IWMPPlayer> m_pWMPPlayer;
	//COcx1 m_player;
	CWMPPlayer4 m_Player;
};
