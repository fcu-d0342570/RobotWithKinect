// Player.cpp : 實作檔
//

#include "stdafx.h"
#include "MFC_TEST.h"
#include "Player.h"
#include "afxdialogex.h"


// Player 對話方塊

IMPLEMENT_DYNAMIC(Player, CDialogEx)

Player::Player(CWnd* pParent /*=NULL*/)
	: CDialogEx(Player::IDD, pParent)
{

}

Player::~Player()
{
}

void Player::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_OCX3, m_Player);
}


BEGIN_MESSAGE_MAP(Player, CDialogEx)
	ON_BN_CLICKED(IDCANCEL, &Player::OnBnClickedCancel)
	ON_BN_CLICKED(IDOK, &Player::OnBnClickedOk)
END_MESSAGE_MAP()


// Player 訊息處理常式


void Player::OnBnClickedCancel()
{
	// TODO:  在此加入控制項告知處理常式程式碼
	CDialogEx::OnCancel();
}


void Player::OnBnClickedOk()
{
	// TODO:  在此加入控制項告知處理常式程式
	CDialogEx::OnOK();
}
