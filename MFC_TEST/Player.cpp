// Player.cpp : ��@��
//

#include "stdafx.h"
#include "MFC_TEST.h"
#include "Player.h"
#include "afxdialogex.h"


// Player ��ܤ��

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


// Player �T���B�z�`��


void Player::OnBnClickedCancel()
{
	// TODO:  �b���[�J����i���B�z�`���{���X
	CDialogEx::OnCancel();
}


void Player::OnBnClickedOk()
{
	// TODO:  �b���[�J����i���B�z�`���{��
	CDialogEx::OnOK();
}
