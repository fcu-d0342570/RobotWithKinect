#pragma once


// Setting 對話方塊

class Setting : public CDialogEx
{
	DECLARE_DYNAMIC(Setting)

public:
	Setting(CWnd* pParent = NULL);   // 標準建構函式
	virtual ~Setting();
	BOOL modeStill = true;	//靜止模式	
	BOOL modeContinus = false;	//連續模式
	BOOL gameMode = true;	//遊戲模式
	int speedHigh = 30;	
	int speedNormal = 60;
	int speedSlow = 100;
	int gameSpeed = 60;	//遊戲速度
	double accuracyHigh = 0.065;	
	double accuracyNormal = 0.073;
	double accuracyLow = 0.081;
	double gameAccuracy = 0.073;	//遊戲精準度
// 對話方塊資料
	enum { IDD = IDD_SETTING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedRadio2();
	afx_msg void OnBnClickedModeContinus();
	afx_msg void OnBnClickedModeStill();
	afx_msg void OnBnClickedEasy();
	afx_msg void OnBnClickedNormal();
	afx_msg void OnBnClickedHard();
	afx_msg void OnBnClickedSpeedHigh();
	afx_msg void OnBnClickedSpeedNormal();
	afx_msg void OnBnClickedSpeedSlow();
};
