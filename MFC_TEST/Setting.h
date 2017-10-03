#pragma once


// Setting ��ܤ��

class Setting : public CDialogEx
{
	DECLARE_DYNAMIC(Setting)

public:
	Setting(CWnd* pParent = NULL);   // �зǫغc�禡
	virtual ~Setting();
	BOOL modeStill = true;	//�R��Ҧ�	
	BOOL modeContinus = false;	//�s��Ҧ�
	BOOL gameMode = true;	//�C���Ҧ�
	int speedHigh = 30;	
	int speedNormal = 60;
	int speedSlow = 100;
	int gameSpeed = 60;	//�C���t��
	double accuracyHigh = 0.065;	
	double accuracyNormal = 0.073;
	double accuracyLow = 0.081;
	double gameAccuracy = 0.073;	//�C����ǫ�
// ��ܤ�����
	enum { IDD = IDD_SETTING };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �䴩

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
