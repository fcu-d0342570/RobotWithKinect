// Setting.cpp : 實作檔
//

#include "stdafx.h"
#include "MFC_TEST.h"
#include "Setting.h"
#include "afxdialogex.h"




// Setting 對話方塊

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


// Setting 訊息處理常式


void Setting::OnBnClickedOk()
{
	// TODO:  在此加入控制項告知處理常式程式碼
	CDialogEx::OnOK();
}


void Setting::OnBnClickedRadio2()
{

	// TODO:  在此加入控制項告知處理常式程式碼
}


void Setting::OnBnClickedModeContinus()
{
	// TODO:  在此加入控制項告知處理常式程式碼
	gameMode = modeContinus;
}


void Setting::OnBnClickedModeStill()
{
	// TODO:  在此加入控制項告知處理常式程式碼
	gameMode = modeStill;
}


void Setting::OnBnClickedEasy()
{
	// TODO:  在此加入控制項告知處理常式程式碼
	gameAccuracy = accuracyLow;
}


void Setting::OnBnClickedNormal()
{
	// TODO:  在此加入控制項告知處理常式程式碼
	gameAccuracy = accuracyNormal;
}


void Setting::OnBnClickedHard()
{
	// TODO:  在此加入控制項告知處理常式程式碼
	gameAccuracy = accuracyHigh;
}


void Setting::OnBnClickedSpeedHigh()
{
	// TODO:  在此加入控制項告知處理常式程式碼
	gameSpeed = speedHigh;
}


void Setting::OnBnClickedSpeedNormal()
{
	// TODO:  在此加入控制項告知處理常式程式碼
	gameSpeed = speedNormal;
}


void Setting::OnBnClickedSpeedSlow()
{
	// TODO:  在此加入控制項告知處理常式程式碼
	gameSpeed = speedSlow;
}
