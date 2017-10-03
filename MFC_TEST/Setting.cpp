// Setting.cpp : ��@��
//

#include "stdafx.h"
#include "MFC_TEST.h"
#include "Setting.h"
#include "afxdialogex.h"




// Setting ��ܤ��

IMPLEMENT_DYNAMIC(Setting, CDialogEx)

Setting::Setting(CWnd* pParent /*=NULL*/)
	: CDialogEx(Setting::IDD, pParent)
{

}

Setting::~Setting()
{
}

void Setting::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(Setting, CDialogEx)
	ON_BN_CLICKED(IDOK, &Setting::OnBnClickedOk)
	ON_BN_CLICKED(IDC_MODE_CONTINUS, &Setting::OnBnClickedModeContinus)
	ON_BN_CLICKED(IDC_MODE_STILL, &Setting::OnBnClickedModeStill)
	ON_BN_CLICKED(IDC_EASY, &Setting::OnBnClickedEasy)
	ON_BN_CLICKED(IDC_NORMAL, &Setting::OnBnClickedNormal)
	ON_BN_CLICKED(IDC_HARD, &Setting::OnBnClickedHard)
	ON_BN_CLICKED(IDC_SPEED_HIGH, &Setting::OnBnClickedSpeedHigh)
	ON_BN_CLICKED(IDC_SPEED_NORMAL, &Setting::OnBnClickedSpeedNormal)
	ON_BN_CLICKED(IDC_SPEED_SLOW, &Setting::OnBnClickedSpeedSlow)
END_MESSAGE_MAP()


// Setting �T���B�z�`��


void Setting::OnBnClickedOk()
{
	// TODO:  �b���[�J����i���B�z�`���{���X
	CDialogEx::OnOK();
}


void Setting::OnBnClickedRadio2()
{

	// TODO:  �b���[�J����i���B�z�`���{���X
}


void Setting::OnBnClickedModeContinus()
{
	// TODO:  �b���[�J����i���B�z�`���{���X
	gameMode = modeContinus;
}


void Setting::OnBnClickedModeStill()
{
	// TODO:  �b���[�J����i���B�z�`���{���X
	gameMode = modeStill;
}


void Setting::OnBnClickedEasy()
{
	// TODO:  �b���[�J����i���B�z�`���{���X
	gameAccuracy = accuracyLow;
}


void Setting::OnBnClickedNormal()
{
	// TODO:  �b���[�J����i���B�z�`���{���X
	gameAccuracy = accuracyNormal;
}


void Setting::OnBnClickedHard()
{
	// TODO:  �b���[�J����i���B�z�`���{���X
	gameAccuracy = accuracyHigh;
}


void Setting::OnBnClickedSpeedHigh()
{
	// TODO:  �b���[�J����i���B�z�`���{���X
	gameSpeed = speedHigh;
}


void Setting::OnBnClickedSpeedNormal()
{
	// TODO:  �b���[�J����i���B�z�`���{���X
	gameSpeed = speedNormal;
}


void Setting::OnBnClickedSpeedSlow()
{
	// TODO:  �b���[�J����i���B�z�`���{���X
	gameSpeed = speedSlow;
}
