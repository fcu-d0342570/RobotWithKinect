
// MFC_TESTDlg.h : 標頭檔
#include <array>
#include <iostream>
#include<time.h>
#include <freeglut.h>
#include <Kinect.h>
#include <fstream>
//

#pragma once
#define RADIAN (3.1415926 / 180.0)


// CMFC_TESTDlg 對話方塊
class CMFC_TESTDlg : public CDialogEx
{
// 建構
public:
	CMFC_TESTDlg(CWnd* pParent = NULL);	// 標準建構函式

// 對話方塊資料
	enum { IDD = IDD_MFC_TEST_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支援


// 程式碼實作
protected:
	HICON m_hIcon;

	// 產生的訊息對應函式
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void btn_record();
public:
	afx_msg void btn_play();
	static UINT THWriteData(LPVOID lParam);
	DWORD output(LPCVOID pData, DWORD len);
	HANDLE hComm;
	double point_body[26][3];
	double vector_body[11][3];
	void ConnectRS232();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnEnChangeEdit1();
	afx_msg void btn_clear();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedOk();
	afx_msg void btn_comparsion();
	afx_msg void btn_result();
	afx_msg void btn_store();
	afx_msg void btn_load();
	afx_msg void OnStnClickedStaticrcount();
	afx_msg void OnFileOpen();
	afx_msg void btn_capture();
	afx_msg void OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void btn_pass();

	
};



class Vector3
{
public:
	float v[3];

public:
	Vector3()
	{
		v[0] = 0;
		v[1] = 0;
		v[2] = 0;
	}

	Vector3(float x, float y, float z)
	{
		v[0] = x;
		v[1] = y;
		v[2] = z;
	}

	Vector3(const Vector3& rV)
	{
		v[0] = rV.v[0];
		v[1] = rV.v[1];
		v[2] = rV.v[2];
	}

	Vector3 operator+(const Vector3& rV2) const
	{
		return Vector3(v[0] + rV2.v[0], v[1] + rV2.v[1], v[2] + rV2.v[2]);
	}

	Vector3 operator-(const Vector3& rV2) const
	{
		return Vector3(v[0] - rV2.v[0], v[1] - rV2.v[1], v[2] - rV2.v[2]);
	}

	Vector3 operator*(float fScale) const
	{
		return Vector3(v[0] * fScale, v[1] * fScale, v[2] * fScale);
	}

	float Length() const
	{
		return sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
	}

	Vector3 Normalize() const
	{
		float fLength = Length();
		return Vector3(v[0] / fLength, v[1] / fLength, v[2] / fLength);
	}

	Vector3 Cross(const Vector3& rV) const
	{
		return Vector3(v[1] * rV.v[2] - v[2] * rV.v[1],
			v[2] * rV.v[0] - v[0] * rV.v[2],
			v[0] * rV.v[1] - v[1] * rV.v[0]);
	}

	float Dot(const Vector3& rV) const
	{
		return v[0] * rV.v[0] + v[1] * rV.v[1] + v[2] * rV.v[2];
	}
};

class SimpleCamera
{
public:
	Vector3	vPosition;
	Vector3	vCenter;
	Vector3	vLook;
	Vector3	vUpper;
	Vector3	vSide;

	void Update()
	{
		vLook = (vCenter - vPosition).Normalize();
		vSide = vLook.Cross(vUpper);
		vUpper = vSide.Cross(vLook);
	}

	void SetCamera()
	{
		Update();
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		gluLookAt(vPosition.v[0], vPosition.v[1], vPosition.v[2],
			vCenter.v[0], vCenter.v[1], vCenter.v[2],
			vUpper.v[0], vUpper.v[1], vUpper.v[2]);
	}

	void MoveSide(float fDistance)
	{
		vPosition = vPosition + vSide * fDistance;
		vCenter = vCenter + vSide * fDistance;
		SetCamera();
	}

	void MoveForward(float fDistance)
	{
		vPosition = vPosition + vLook * fDistance;
		vCenter = vCenter + vLook * fDistance;
		SetCamera();
	}

	void MoveUp(float fDistance)
	{
		vPosition = vPosition + vUpper * fDistance;
		vCenter = vCenter + vUpper * fDistance;
		SetCamera();
	}

	void RotateUp(float fR)
	{
		fR *= (vPosition - vCenter).Length();
		vCenter = vCenter + vUpper * fR;
		SetCamera();
	}

	void RotateSide(float fR)
	{
		fR *= (vPosition - vCenter).Length();
		vCenter = vCenter + vSide * fR;
		SetCamera();
	}
};
