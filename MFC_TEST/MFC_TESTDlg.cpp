
// MFC_TESTDlg.cpp : 實作檔
//

#include "stdafx.h"
#include "MFC_TEST.h"
#include "MFC_TESTDlg.h"
#include "afxdialogex.h"
#include "Setting.h"
#include "Player.h"
#include <mmsystem.h>
#include <windows.h>
#define MAX_CHAR        128

#pragma comment(lib, "winmm.lib") 


using namespace std;

// global objects
IKinectSensor*		pSensor = nullptr;
IBodyFrameReader*	pBodyFrameReader = nullptr;

Player player;
Setting setting;
INT32				iBodyCount = 0;
IBody**				aBody = nullptr;
int					rCount = 0;
int				recordFlag = 0;		//錄影
int				replayFlag = 0;		//撥放
int				checkFlag = 0;		//比對
int				captureFlag = 0;	//擷取畫面
int				dynamicFlag = 0;
int				checkCount = 0;
int				delayTime = 35;		//撥放延遲時間
float			X1[24];
float			Y1[24];
float			Z1[24];
float			X2[24];
float			Y2[24];
float			Z2[24];
double vector_body[11][3];
double point_body[26][3];
int				passCount = 0;	//成功張數
int				totalCount = 0;
int				Dx = 1, Dy = 1, Dz = 1;
double plane_a, plane_b, plane_c, plane_t, plane_d;
double plane_a2, plane_b2, plane_c2, plane_t2, plane_d2;
char Send_msg[64];
int char_lenth;
CWinThread *m_pThreadA;
CWinThread *m_pThreadB;
CString Send_commend;
CString joint_commend[12];
BOOL connected = false;
CString m_strFilePathName;	//檔案路徑
Joint rJoints[40000][JointType::JointType_Count];	//暫存錄影資料
SimpleCamera g_Camera;
void Send(CString Str);
void Convert2Send(double deg, int joint);

enum Vector_type
{
	Vector_Base = 0,
	Vector_Shouder = 1,
	Vector_HandR_Top = 2,
	Vector_HandR_Bot = 3,
	Vector_HandL_Top = 4,
	Vector_HandL_Bot = 5,
	Vector_LegR_Top = 6,
	Vector_LegR_Bot = 7,
	Vector_LegL_Top = 8,
	Vector_LegL_Bot = 9,
	Vector_Hip = 10,
};



#ifdef _DEBUG
#define new DEBUG_NEW
#endif



// 對 App About 使用 CAboutDlg 對話方塊

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 對話方塊資料
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援

// 程式碼實作
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMFC_TESTDlg 對話方塊



CMFC_TESTDlg::CMFC_TESTDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMFC_TESTDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CMFC_TESTDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CMFC_TESTDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CMFC_TESTDlg::btn_record)
	ON_BN_CLICKED(IDC_BUTTON2, &CMFC_TESTDlg::btn_play)
	ON_EN_CHANGE(IDC_EDIT1, &CMFC_TESTDlg::OnEnChangeEdit1)
	ON_BN_CLICKED(IDC_BUTTON3, &CMFC_TESTDlg::btn_clear)
	ON_BN_CLICKED(IDCANCEL, &CMFC_TESTDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDOK, &CMFC_TESTDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON4, &CMFC_TESTDlg::btn_comparsion)
	ON_BN_CLICKED(IDC_BUTTON5, &CMFC_TESTDlg::btn_result)
	ON_BN_CLICKED(IDC_BUTTON6, &CMFC_TESTDlg::btn_store)
	ON_BN_CLICKED(IDC_BUTTON7, &CMFC_TESTDlg::btn_load)
	ON_STN_CLICKED(IDC_STATIC_rCount, &CMFC_TESTDlg::OnStnClickedStaticrcount)
	ON_BN_CLICKED(IDC_btn_capture, &CMFC_TESTDlg::btn_capture)
	ON_BN_CLICKED(IDC_BUTTON8, &CMFC_TESTDlg::btn_pass)
END_MESSAGE_MAP()


// CMFC_TESTDlg 訊息處理常式

BOOL CMFC_TESTDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 將 [關於...] 功能表加入系統功能表。

	// IDM_ABOUTBOX 必須在系統命令範圍之中。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 設定此對話方塊的圖示。當應用程式的主視窗不是對話方塊時，
	// 框架會自動從事此作業
	SetIcon(m_hIcon, TRUE);			// 設定大圖示
	SetIcon(m_hIcon, FALSE);		// 設定小圖示
	SetDlgItemText(IDC_STATIC_MODE, L"靜止");
	SetDlgItemText(IDC_STATIC_SPEED, L"普通");
	SetDlgItemText(IDC_STATIC_DIFFCULITY, L"普通");
	// TODO:  在此加入額外的初始設定
	ConnectRS232();
	return TRUE;  // 傳回 TRUE，除非您對控制項設定焦點
}

void CMFC_TESTDlg::OnTimer(UINT_PTR nIDEvent)
{

	char temp1[100];
	_itoa_s(passCount, temp1, 10);
	CString csStr1(temp1);
	SetDlgItemText(IDC_STATIC_cCount, csStr1);
	char temp2[100];
	_itoa_s(rCount, temp2, 10);
	CString csStr2(temp2);
	SetDlgItemText(IDC_STATIC_rCount, csStr2);
	char temp3[100];
	int tempScore;
	tempScore = ((double)passCount / (double)rCount) * 100;
	CString str;
	str.Format(_T("%d"), tempScore);
	_itoa_s(tempScore, temp3, 10);
	CString csStr3(temp3);
	SetDlgItemText(IDC_STATIC_SCORE, csStr3);
	GetDlgItem(IDC_SCORE)->SetWindowTextW(str);


	CDialogEx::OnTimer(nIDEvent);
}

void CMFC_TESTDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

void DrawLine(const Joint& rJ1, const Joint& rJ2)
{
	//if (rJ1.TrackingState == TrackingState_NotTracked || rJ2.TrackingState == TrackingState_NotTracked)
		//return;

	glVertex3f(rJ1.Position.X, rJ1.Position.Y, rJ1.Position.Z);
	glVertex3f(rJ2.Position.X, rJ2.Position.Y, rJ2.Position.Z);
	//紀錄資訊

	switch (rJ2.JointType)
	{
	case JointType_SpineBase:
		vector_body[Vector_Base][0] = rJ1.Position.X - rJ2.Position.X;
		vector_body[Vector_Base][1] = rJ1.Position.Y - rJ2.Position.Y;
		vector_body[Vector_Base][2] = rJ1.Position.Z - rJ2.Position.Z;
		point_body[JointType_SpineBase][0] = rJ2.Position.X;
		point_body[JointType_SpineBase][1] = rJ2.Position.Y;
		point_body[JointType_SpineBase][2] = rJ2.Position.Z;
		break;
	case JointType_SpineMid:
		point_body[JointType_SpineBase][0] = rJ2.Position.X;
		point_body[JointType_SpineBase][1] = rJ2.Position.Y;
		point_body[JointType_SpineBase][2] = rJ2.Position.Z;
		break;
	case JointType_ElbowRight:
		vector_body[Vector_HandR_Top][0] = rJ1.Position.X - rJ2.Position.X;
		vector_body[Vector_HandR_Top][1] = rJ1.Position.Y - rJ2.Position.Y;
		vector_body[Vector_HandR_Top][2] = rJ1.Position.Z - rJ2.Position.Z;
		point_body[JointType_ElbowRight][0] = rJ2.Position.X;
		point_body[JointType_ElbowRight][1] = rJ2.Position.Y;
		point_body[JointType_ElbowRight][2] = rJ2.Position.Z;
		break;
	case JointType_WristRight:
		vector_body[Vector_HandR_Bot][0] = rJ1.Position.X - rJ2.Position.X;
		vector_body[Vector_HandR_Bot][1] = rJ1.Position.Y - rJ2.Position.Y;
		vector_body[Vector_HandR_Bot][2] = rJ1.Position.Z - rJ2.Position.Z;
		break;
	case JointType_ElbowLeft:
		vector_body[Vector_HandL_Top][0] = rJ1.Position.X - rJ2.Position.X;
		vector_body[Vector_HandL_Top][1] = rJ1.Position.Y - rJ2.Position.Y;
		vector_body[Vector_HandL_Top][2] = rJ1.Position.Z - rJ2.Position.Z;
		point_body[JointType_ElbowLeft][0] = rJ2.Position.X;
		point_body[JointType_ElbowLeft][1] = rJ2.Position.Y;
		point_body[JointType_ElbowLeft][2] = rJ2.Position.Z;
		break;
	case JointType_WristLeft:
		vector_body[Vector_HandL_Bot][0] = rJ1.Position.X - rJ2.Position.X;
		vector_body[Vector_HandL_Bot][1] = rJ1.Position.Y - rJ2.Position.Y;
		vector_body[Vector_HandL_Bot][2] = rJ1.Position.Z - rJ2.Position.Z;
		break;

	case JointType_ShoulderRight:
		plane_a = rJ1.Position.X - rJ2.Position.X;
		plane_b = rJ1.Position.Y - rJ2.Position.Y;
		plane_c = rJ1.Position.Z - rJ2.Position.Z;
		plane_d = -(plane_a*point_body[JointType_SpineShoulder][0] + plane_b*point_body[JointType_SpineShoulder][1] + plane_c*point_body[JointType_SpineShoulder][2]);
		break;

	case JointType_SpineShoulder:
		point_body[JointType_SpineShoulder][0] = rJ2.Position.X;
		point_body[JointType_SpineShoulder][1] = rJ2.Position.Y;
		point_body[JointType_SpineShoulder][2] = rJ2.Position.Z;
		break;
	default:
		break;
	}
}


float getVectorX(const Joint& rJ1, const Joint& rJ2) {//向量比對
	return (rJ1.Position.X - rJ2.Position.X);
}

float getVectorY(const Joint& rJ1, const Joint& rJ2) {
	return (rJ1.Position.Y - rJ2.Position.Y);
}

float getVectorZ(const Joint& rJ1, const Joint& rJ2) {
	return (rJ1.Position.Z - rJ2.Position.Z);
}

bool getVResult(float* X1, float* Y1, float* Z1, float* X2, float* Y2, float* Z2) {
	for (int i = 0; i < 24; i++) {
		if ((abs(X1[i] - X2[i]) > setting.gameAccuracy) || (abs(Y1[i] - Y2[i]) > setting.gameAccuracy) || (abs(Z1[i] - Z2[i]) > setting.gameAccuracy)) return false;
	}
	return true;
}

const char* getScore() {
	int tempScore;
	tempScore = ((double)passCount / (double)rCount) * 100;
	char score[10];
	_itoa_s(tempScore, score, 10);
	return score;
}

void selectFont(int size, int charset, const char* face) {
	HFONT hFont = CreateFontA(size, 0, 0, 0, FW_MEDIUM, 0, 0, 0,
		charset, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, face);
	HFONT hOldFont = (HFONT)SelectObject(wglGetCurrentDC(), hFont);
	DeleteObject(hOldFont);
}

void drawString(const char* str) {
	static int isFirstCall = 1;
	static GLuint lists;

	if (isFirstCall) {

		isFirstCall = 0;


		lists = glGenLists(MAX_CHAR);


		wglUseFontBitmaps(wglGetCurrentDC(), 0, MAX_CHAR, lists);
	}

	for (; *str != '\0'; ++str)
		glCallList(lists + *str);
}

void CountRightH(){

	CString str;

	//以肩膀為法向量之身體平面
	plane_t = (plane_a*point_body[JointType_ElbowRight][0] + plane_b*point_body[JointType_ElbowRight][1] + plane_c*point_body[JointType_ElbowRight][2] + plane_d) /
		(plane_a*plane_a + plane_b*plane_b + plane_c*plane_c);
	double x1, y1, z1;

	//上手臂投影於身體平面
	x1 = point_body[JointType_ElbowRight][0] - plane_a*plane_t;
	y1 = point_body[JointType_ElbowRight][1] - plane_b*plane_t;
	z1 = point_body[JointType_ElbowRight][2] - plane_c*plane_t;

	x1 = x1 - point_body[JointType_SpineShoulder][0];
	y1 = y1 - point_body[JointType_SpineShoulder][1];
	z1 = z1 - point_body[JointType_SpineShoulder][2];

	double T1, T2, T3;
	//上手臂與身體角度
	T1 = acos((x1*vector_body[Vector_Base][0] + y1*vector_body[Vector_Base][1] + z1*vector_body[Vector_Base][2]) / (sqrt(x1*x1 + y1*y1 + z1*z1)*
		sqrt(vector_body[Vector_Base][0] * vector_body[Vector_Base][0] + vector_body[Vector_Base][1] * vector_body[Vector_Base][1] + vector_body[Vector_Base][2] * vector_body[Vector_Base][2]))) / RADIAN;

	T2 = acos((x1*vector_body[Vector_HandR_Top][0] + y1*vector_body[Vector_HandR_Top][1] + z1*vector_body[Vector_HandR_Top][2]) / (sqrt(x1*x1 + y1*y1 + z1*z1)*
		sqrt(vector_body[Vector_HandR_Top][0] * vector_body[Vector_HandR_Top][0] + vector_body[Vector_HandR_Top][1] * vector_body[Vector_HandR_Top][1] + vector_body[Vector_HandR_Top][2] * vector_body[Vector_HandR_Top][2]))) / RADIAN;
	if (z1 >= 0)
		T1 = (180 - T1) + 180;

	//手軸角度
	T3 = acos((vector_body[Vector_HandR_Bot][0] * vector_body[Vector_HandR_Top][0] + vector_body[Vector_HandR_Bot][1] * vector_body[Vector_HandR_Top][1] + vector_body[Vector_HandR_Bot][2] * vector_body[Vector_HandR_Top][2]) /
		(sqrt(vector_body[Vector_HandR_Bot][0] * vector_body[Vector_HandR_Bot][0] + vector_body[Vector_HandR_Bot][1] * vector_body[Vector_HandR_Bot][1] + vector_body[Vector_HandR_Bot][2] * vector_body[Vector_HandR_Bot][2])*
		sqrt(vector_body[Vector_HandR_Top][0] * vector_body[Vector_HandR_Top][0] + vector_body[Vector_HandR_Top][1] * vector_body[Vector_HandR_Top][1] + vector_body[Vector_HandR_Top][2] * vector_body[Vector_HandR_Top][2]))) / RADIAN;

	//將計算之角度轉換為命令
	Convert2Send(T1, 4);
	Convert2Send(T2, 5);
	Convert2Send(T3, 6);

}

void CountLeftH(){

	CString str;


	plane_t = (plane_a*point_body[JointType_ElbowLeft][0] + plane_b*point_body[JointType_ElbowLeft][1] + plane_c*point_body[JointType_ElbowLeft][2] + plane_d) /
		(plane_a*plane_a + plane_b*plane_b + plane_c*plane_c);


	double x1, y1, z1;
	x1 = point_body[JointType_ElbowLeft][0] - plane_a*plane_t;
	y1 = point_body[JointType_ElbowLeft][1] - plane_b*plane_t;
	z1 = point_body[JointType_ElbowLeft][2] - plane_c*plane_t;

	x1 = x1 - point_body[JointType_SpineShoulder][0];
	y1 = y1 - point_body[JointType_SpineShoulder][1];
	z1 = z1 - point_body[JointType_SpineShoulder][2];

	double T1, T2, T3;
	T1 = acos((x1*vector_body[Vector_Base][0] + y1*vector_body[Vector_Base][1] + z1*vector_body[Vector_Base][2]) / (sqrt(x1*x1 + y1*y1 + z1*z1)*
		sqrt(vector_body[Vector_Base][0] * vector_body[Vector_Base][0] + vector_body[Vector_Base][1] * vector_body[Vector_Base][1] + vector_body[Vector_Base][2] * vector_body[Vector_Base][2]))) / RADIAN;
	T2 = acos((x1*vector_body[Vector_HandL_Top][0] + y1*vector_body[Vector_HandL_Top][1] + z1*vector_body[Vector_HandL_Top][2]) / (sqrt(x1*x1 + y1*y1 + z1*z1)*
		sqrt(vector_body[Vector_HandL_Top][0] * vector_body[Vector_HandL_Top][0] + vector_body[Vector_HandL_Top][1] * vector_body[Vector_HandL_Top][1] + vector_body[Vector_HandL_Top][2] * vector_body[Vector_HandL_Top][2]))) / RADIAN;
	if (z1 >= 0)
		T1 = (180 - T1) + 180;

	T3 = acos((vector_body[Vector_HandL_Bot][0] * vector_body[Vector_HandL_Top][0] + vector_body[Vector_HandL_Bot][1] * vector_body[Vector_HandL_Top][1] + vector_body[Vector_HandL_Bot][2] * vector_body[Vector_HandL_Top][2]) /
		(sqrt(vector_body[Vector_HandL_Bot][0] * vector_body[Vector_HandL_Bot][0] + vector_body[Vector_HandL_Bot][1] * vector_body[Vector_HandL_Bot][1] + vector_body[Vector_HandL_Bot][2] * vector_body[Vector_HandL_Bot][2])*
		sqrt(vector_body[Vector_HandL_Top][0] * vector_body[Vector_HandL_Top][0] + vector_body[Vector_HandL_Top][1] * vector_body[Vector_HandL_Top][1] + vector_body[Vector_HandL_Top][2] * vector_body[Vector_HandL_Top][2]))) / RADIAN;

	Convert2Send(T1, 8);
	Convert2Send(T2, 9);
	Convert2Send(T3, 10);

}


void Convert2Send(double deg, int joint)
{
	double temp;
	int temp2;
	CString str;
	switch (joint)
	{
	case 4:
		if (deg < 30)
			deg = 30;
		//微調角度
		temp = (2200.0 - ((deg - 30) / 170.0)*1400.0) / 10.0;
		temp2 = temp;
		if (temp2 < 80)
			joint_commend[3].Format(_T("080"));
		else if (temp2 < 100)
			joint_commend[3].Format(_T("0%d"), temp2);
		else if (temp2 >= 220)
			joint_commend[3].Format(_T("220"));
		else
			joint_commend[3].Format(_T("%d"), temp2);
		break;
	case 5:
		temp = (2200.0 - (deg / 180.0)*1400.0) / 10.0;
		temp2 = temp;
		if (temp2 < 80)
			joint_commend[4].Format(_T("080"));
		else if (temp2 < 100)
			joint_commend[4].Format(_T("0%d"), temp2);
		else if (temp2 >= 220)
			joint_commend[4].Format(_T("220"));
		else
			joint_commend[4].Format(_T("%d"), temp2);
		break;
	case 6:
		temp = (2200.0 - (deg / 160.0)*1400.0) / 10.0;

		temp2 = temp - 75;
		if (temp2 < 80)
			joint_commend[5].Format(_T("080"));
		else if (temp2 < 100)
			joint_commend[5].Format(_T("0%d"), temp2);
		else if (temp2 >= 220)
			joint_commend[5].Format(_T("220"));
		else
			joint_commend[5].Format(_T("%d"), temp2);
		break;


	case 8:
		if (deg < 30)
			deg = 30;
		temp = ((deg - 30) / 170.0*1400.0 + 800.0) / 10.0;
		temp2 = temp;
		if (temp2 < 80)
			joint_commend[7].Format(_T("080"));
		else if (temp2 < 100)
			joint_commend[7].Format(_T("0%d"), temp2);
		else if (temp2 >= 220)
			joint_commend[7].Format(_T("220"));
		else
			joint_commend[7].Format(_T("%d"), temp2);
		break;
	case 9:
		temp = ((deg) / 180.0*1400.0 + 800.0) / 10.0;
		temp2 = temp;
		if (temp2 < 80)
			joint_commend[8].Format(_T("080"));
		else if (temp2 < 100)
			joint_commend[8].Format(_T("0%d"), temp2);
		else if (temp2 >= 220)
			joint_commend[8].Format(_T("220"));
		else
			joint_commend[8].Format(_T("%d"), temp2);
		break;
	case 10:
		temp = ((deg) / 160.0*1400.0 + 800.0) / 10.0;
		temp2 = temp + 75;
		if (temp2 < 80)
			joint_commend[9].Format(_T("080"));
		else if (temp2 < 100)
			joint_commend[9].Format(_T("0%d"), temp2);
		else if (temp2 >= 220)
			joint_commend[9].Format(_T("220"));
		else
			joint_commend[9].Format(_T("%d"), temp2);
		break;
	default:
		break;
	}
}


//送出命令
void SendComend(){
	Send_commend = joint_commend[3] + joint_commend[4] + joint_commend[5] + joint_commend[7] + joint_commend[8] + joint_commend[9];
	//Send_commend = joint_commend[3] + joint_commend[4] + joint_commend[5];
	Send(Send_commend);
}

/*******************RS232 連線設定********************/

void CMFC_TESTDlg::ConnectRS232()
{
	if (connected)
	{
		//SetDlgItemTextW(IDC_CONNECT, _T("Connect"));
		CloseHandle(hComm);
		connected = false;
	}
	else
	{
		DCB dcb;
		COMMTIMEOUTS gTimeoutsDefault = { 0, 0, 2000, 0, 2000 };
		COMMTIMEOUTS m_timeoutorig;
		hComm = CreateFile(_T("\\\\.\\COM3"), GENERIC_READ | GENERIC_WRITE, 0,
			0, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, 0);
		PurgeComm(hComm, PURGE_RXABORT | PURGE_RXCLEAR);
		if ((hComm != NULL) && (hComm != INVALID_HANDLE_VALUE)){
			//MessageBox(_T("Sucess"));
			//SetDlgItemTextW(IDC_CONNECT, _T("Disconnect"));
			connected = true;
			GetCommTimeouts(hComm, &m_timeoutorig);
			dcb.DCBlength = sizeof(dcb);
			GetCommState(hComm, &dcb); // get current DCB settings
			dcb.BaudRate = CBR_9600;   // baud rate 9600
			//dcb.BaudRate = CBR_38400;

			dcb.ByteSize = 8;          // 字元位元數 8 bits
			dcb.Parity = NOPARITY; // even parity 位元
			dcb.StopBits = ONESTOPBIT; // 結束位元數 1 bit
			dcb.fParity = 0;
			dcb.EvtChar = (char)0x0A;
			dcb.EofChar = (char)0x0D;
			if (!SetCommState(hComm, &dcb)) {	// 新設定
				AfxMessageBox(_T("Error : RS232 Comport initial fail"));
				CloseHandle(hComm);
				hComm = NULL;
			}
			else
				SetCommTimeouts(hComm, &gTimeoutsDefault);	// 設定新 timeouts
		}
		else
		{
			AfxMessageBox(_T("Error : RS232 Comport Open fail"));
			return;
		}

		DWORD dwModemStatus, dwErrors;
		GetCommModemStatus(hComm, &dwModemStatus);
		ClearCommError(hComm, &dwErrors, NULL);
		COMSTAT stat;
		ClearCommError(hComm, &dwErrors, &stat);
		SetupComm(hComm, 10000, 10000);

		COMMTIMEOUTS Timeouts;
		GetCommTimeouts(hComm, &Timeouts);

		Timeouts.ReadIntervalTimeout = 0;
		Timeouts.ReadTotalTimeoutMultiplier = 5;
		Timeouts.ReadTotalTimeoutConstant = 20;
		Timeouts.WriteTotalTimeoutMultiplier = 5;
		Timeouts.WriteTotalTimeoutConstant = 20;

		SetCommTimeouts(hComm, &Timeouts);
	}
}

/*UINT THWriteData(LPVOID lParam)
{
	CMFC_TESTDlg* hWnd = (CMFC_TESTDlg *)lParam;

	hWnd->output(hWnd->Send_msg, hWnd->char_lenth + 2);
	return 0;
}*/

DWORD CMFC_TESTDlg::output(LPCVOID pData, DWORD len)
{
	OVERLAPPED osWrite = { 0 };
	DWORD dwWritten = 0;
	DWORD dwRes, dwLoops = TRUE;
	osWrite.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (osWrite.hEvent != NULL) {
		while (dwLoops) {
			//WaitForSingleObject(g_StartWrite.m_hObject, INFINITE);
			if (!WriteFile(hComm, pData, len, &dwWritten, &osWrite)) { // issue write
				if (GetLastError() == ERROR_IO_PENDING) {	// write is delayed 
					dwRes = WaitForSingleObject(osWrite.hEvent, 2000); //2000);
					switch (dwRes) {
					case WAIT_OBJECT_0:	// written
						GetOverlappedResult(hComm, &osWrite, &dwWritten, FALSE);
						dwLoops = FALSE;
						break;
					case WAIT_TIMEOUT:	// wait timed out
						break;
					default:
						break;
					}
				}
			}
			else{
				GetOverlappedResult(hComm, &osWrite, &dwWritten, FALSE);
				dwLoops = FALSE;
			}
		}
		CloseHandle(osWrite.hEvent);
	}
	return 2;
}

void Send(CString Str)
{
	int i;

	char_lenth = Str.GetLength();
	for (i = 0; i < 64; i++)
	{
		Send_msg[i] = NULL;
	}

	for (i = 0; i < char_lenth; i++)
	{
		Send_msg[i] = Str.GetAt(i);
	}
	Send_msg[i] = 0X0D;
	Send_msg[i + 1] = 0X0A;
	//m_pThreadB = AfxBeginThread(THWriteData, this);

}

void Rec(int count) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// draw points
	selectFont(64, ANSI_CHARSET, "Comic Sans MS");
	glColor3f(0.0f, 1.0f, 0.0f);
	glRasterPos2f(0.0f, 0.7f);
	Sleep(delayTime);
	drawString("Replay");


	glPointSize(1.0f);
	glLineWidth(5.0f);
	glBegin(GL_LINES);
	glColor3ub(0, 255, 0);
	
	DrawLine(rJoints[count][JointType_SpineBase], rJoints[count][JointType_SpineMid]);
	DrawLine(rJoints[count][JointType_SpineBase], rJoints[count][JointType_SpineMid]);
	DrawLine(rJoints[count][JointType_SpineMid], rJoints[count][JointType_SpineShoulder]);
	DrawLine(rJoints[count][JointType_SpineShoulder], rJoints[count][JointType_Neck]);
	DrawLine(rJoints[count][JointType_Neck], rJoints[count][JointType_Head]);

	DrawLine(rJoints[count][JointType_SpineShoulder], rJoints[count][JointType_ShoulderLeft]);
	DrawLine(rJoints[count][JointType_ShoulderLeft], rJoints[count][JointType_ElbowLeft]);
	DrawLine(rJoints[count][JointType_ElbowLeft], rJoints[count][JointType_WristLeft]);
	DrawLine(rJoints[count][JointType_WristLeft], rJoints[count][JointType_HandLeft]);

	DrawLine(rJoints[count][JointType_SpineShoulder], rJoints[count][JointType_ShoulderRight]);
	DrawLine(rJoints[count][JointType_ShoulderRight], rJoints[count][JointType_ElbowRight]);
	DrawLine(rJoints[count][JointType_ElbowRight], rJoints[count][JointType_WristRight]);
	DrawLine(rJoints[count][JointType_WristRight], rJoints[count][JointType_HandRight]);

	DrawLine(rJoints[count][JointType_SpineBase], rJoints[count][JointType_HipLeft]);
	DrawLine(rJoints[count][JointType_HipLeft], rJoints[count][JointType_KneeLeft]);
	DrawLine(rJoints[count][JointType_KneeLeft], rJoints[count][JointType_AnkleLeft]);

	DrawLine(rJoints[count][JointType_SpineBase], rJoints[count][JointType_HipRight]);
	DrawLine(rJoints[count][JointType_HipRight], rJoints[count][JointType_KneeRight]);
	DrawLine(rJoints[count][JointType_KneeRight], rJoints[count][JointType_AnkleRight]);

	glEnd();

	glutSwapBuffers();
}

void dynamicPlay(int count, float Magnification) {
	glLineWidth(Magnification);
	glBegin(GL_LINES);
	glColor3f(0, 1, 0);

	DrawLine(rJoints[count][JointType_SpineBase], rJoints[count][JointType_SpineMid]);

	DrawLine(rJoints[count][JointType_SpineMid], rJoints[count][JointType_SpineShoulder]);

	DrawLine(rJoints[count][JointType_SpineShoulder], rJoints[count][JointType_Neck]);

	DrawLine(rJoints[count][JointType_Neck], rJoints[count][JointType_Head]);

	DrawLine(rJoints[count][JointType_SpineShoulder], rJoints[count][JointType_ShoulderLeft]);

	DrawLine(rJoints[count][JointType_ShoulderLeft], rJoints[count][JointType_ElbowLeft]);

	DrawLine(rJoints[count][JointType_ElbowLeft], rJoints[count][JointType_WristLeft]);

	DrawLine(rJoints[count][JointType_WristLeft], rJoints[count][JointType_HandLeft]);

	DrawLine(rJoints[count][JointType_SpineShoulder], rJoints[count][JointType_ShoulderRight]);

	DrawLine(rJoints[count][JointType_ShoulderRight], rJoints[count][JointType_ElbowRight]);


	DrawLine(rJoints[count][JointType_ElbowRight], rJoints[count][JointType_WristRight]);

	DrawLine(rJoints[count][JointType_WristRight], rJoints[count][JointType_HandRight]);


	DrawLine(rJoints[count][JointType_SpineBase], rJoints[count][JointType_HipLeft]);


	DrawLine(rJoints[count][JointType_HipLeft], rJoints[count][JointType_KneeLeft]);


	DrawLine(rJoints[count][JointType_KneeLeft], rJoints[count][JointType_AnkleLeft]);

	DrawLine(rJoints[count][JointType_SpineBase], rJoints[count][JointType_HipRight]);


	DrawLine(rJoints[count][JointType_HipRight], rJoints[count][JointType_KneeRight]);


	DrawLine(rJoints[count][JointType_KneeRight], rJoints[count][JointType_AnkleRight]);

	glEnd();
}

void drawBody(int count) {
	glLineWidth(5.0f);
	glBegin(GL_LINES);
	glColor3f(0, 1, 0);

	DrawLine(rJoints[count][JointType_SpineBase], rJoints[count][JointType_SpineMid]);
	X1[0] = getVectorX(rJoints[count][JointType_SpineBase], rJoints[count][JointType_SpineMid]);
	Y1[0] = getVectorY(rJoints[count][JointType_SpineBase], rJoints[count][JointType_SpineMid]);
	Z1[0] = getVectorZ(rJoints[count][JointType_SpineBase], rJoints[count][JointType_SpineMid]);

	DrawLine(rJoints[count][JointType_SpineMid], rJoints[count][JointType_SpineShoulder]);
	X1[1] = getVectorX(rJoints[count][JointType_SpineMid], rJoints[count][JointType_SpineShoulder]);
	Y1[1] = getVectorY(rJoints[count][JointType_SpineMid], rJoints[count][JointType_SpineShoulder]);
	Z1[1] = getVectorZ(rJoints[count][JointType_SpineMid], rJoints[count][JointType_SpineShoulder]);

	DrawLine(rJoints[count][JointType_SpineShoulder], rJoints[count][JointType_Neck]);
	X1[2] = getVectorX(rJoints[count][JointType_SpineShoulder], rJoints[count][JointType_Neck]);
	Y1[2] = getVectorY(rJoints[count][JointType_SpineShoulder], rJoints[count][JointType_Neck]);
	Z1[2] = getVectorZ(rJoints[count][JointType_SpineShoulder], rJoints[count][JointType_Neck]);

	DrawLine(rJoints[count][JointType_Neck], rJoints[count][JointType_Head]);
	X1[3] = getVectorX(rJoints[count][JointType_Neck], rJoints[count][JointType_Head]);
	Y1[3] = getVectorY(rJoints[count][JointType_Neck], rJoints[count][JointType_Head]);
	Z1[3] = getVectorZ(rJoints[count][JointType_Neck], rJoints[count][JointType_Head]);

	DrawLine(rJoints[count][JointType_SpineShoulder], rJoints[count][JointType_ShoulderLeft]);
	X1[4] = getVectorX(rJoints[count][JointType_SpineShoulder], rJoints[count][JointType_ShoulderLeft]);
	Y1[4] = getVectorY(rJoints[count][JointType_SpineShoulder], rJoints[count][JointType_ShoulderLeft]);
	Z1[4] = getVectorZ(rJoints[count][JointType_SpineShoulder], rJoints[count][JointType_ShoulderLeft]);

	DrawLine(rJoints[count][JointType_ShoulderLeft], rJoints[count][JointType_ElbowLeft]);
	X1[5] = getVectorX(rJoints[count][JointType_ShoulderLeft], rJoints[count][JointType_ElbowLeft]);
	Y1[5] = getVectorY(rJoints[count][JointType_ShoulderLeft], rJoints[count][JointType_ElbowLeft]);
	Z1[5] = getVectorZ(rJoints[count][JointType_ShoulderLeft], rJoints[count][JointType_ElbowLeft]);

	DrawLine(rJoints[count][JointType_ElbowLeft], rJoints[count][JointType_WristLeft]);
	X1[6] = getVectorX(rJoints[count][JointType_ElbowLeft], rJoints[count][JointType_WristLeft]);
	Y1[6] = getVectorY(rJoints[count][JointType_ElbowLeft], rJoints[count][JointType_WristLeft]);
	Z1[6] = getVectorZ(rJoints[count][JointType_ElbowLeft], rJoints[count][JointType_WristLeft]);

	DrawLine(rJoints[count][JointType_WristLeft], rJoints[count][JointType_HandLeft]);
	X1[7] = getVectorX(rJoints[count][JointType_WristLeft], rJoints[count][JointType_HandLeft]);
	Y1[7] = getVectorY(rJoints[count][JointType_WristLeft], rJoints[count][JointType_HandLeft]);
	Z1[7] = getVectorZ(rJoints[count][JointType_WristLeft], rJoints[count][JointType_HandLeft]);

	DrawLine(rJoints[count][JointType_SpineShoulder], rJoints[count][JointType_ShoulderRight]);
	X1[10] = getVectorX(rJoints[count][JointType_SpineShoulder], rJoints[count][JointType_ShoulderRight]);
	Y1[10] = getVectorY(rJoints[count][JointType_SpineShoulder], rJoints[count][JointType_ShoulderRight]);
	Z1[10] = getVectorZ(rJoints[count][JointType_SpineShoulder], rJoints[count][JointType_ShoulderRight]);

	DrawLine(rJoints[count][JointType_ShoulderRight], rJoints[count][JointType_ElbowRight]);
	X1[11] = getVectorX(rJoints[count][JointType_ShoulderRight], rJoints[count][JointType_ElbowRight]);
	Y1[11] = getVectorY(rJoints[count][JointType_ShoulderRight], rJoints[count][JointType_ElbowRight]);
	Z1[11] = getVectorZ(rJoints[count][JointType_ShoulderRight], rJoints[count][JointType_ElbowRight]);

	DrawLine(rJoints[count][JointType_ElbowRight], rJoints[count][JointType_WristRight]);
	X1[12] = getVectorX(rJoints[count][JointType_ElbowRight], rJoints[count][JointType_WristRight]);
	Y1[12] = getVectorY(rJoints[count][JointType_ElbowRight], rJoints[count][JointType_WristRight]);
	Z1[12] = getVectorZ(rJoints[count][JointType_ElbowRight], rJoints[count][JointType_WristRight]);

	DrawLine(rJoints[count][JointType_WristRight], rJoints[count][JointType_HandRight]);
	X1[13] = getVectorX(rJoints[count][JointType_WristRight], rJoints[count][JointType_HandRight]);
	Y1[13] = getVectorY(rJoints[count][JointType_WristRight], rJoints[count][JointType_HandRight]);
	Z1[13] = getVectorZ(rJoints[count][JointType_WristRight], rJoints[count][JointType_HandRight]);

	DrawLine(rJoints[count][JointType_SpineBase], rJoints[count][JointType_HipLeft]);
	//X1[16] = getVectorX(rJoints[count][JointType_SpineBase], rJoints[count][JointType_HipLeft]);
	//Y1[16] = getVectorY(rJoints[count][JointType_SpineBase], rJoints[count][JointType_HipLeft]);
	//Z1[16] = getVectorZ(rJoints[count][JointType_SpineBase], rJoints[count][JointType_HipLeft]);

	DrawLine(rJoints[count][JointType_HipLeft], rJoints[count][JointType_KneeLeft]);
	//X1[17] = getVectorX(rJoints[count][JointType_HipLeft], rJoints[count][JointType_KneeLeft]);
	//Y1[17] = getVectorY(rJoints[count][JointType_HipLeft], rJoints[count][JointType_KneeLeft]);
	//Z1[17] = getVectorZ(rJoints[count][JointType_HipLeft], rJoints[count][JointType_KneeLeft]);

	DrawLine(rJoints[count][JointType_KneeLeft], rJoints[count][JointType_AnkleLeft]);
	//X1[18] = getVectorX(rJoints[count][JointType_KneeLeft], rJoints[count][JointType_AnkleLeft]);
	//Y1[18] = getVectorY(rJoints[count][JointType_KneeLeft], rJoints[count][JointType_AnkleLeft]);
	//Z1[18] = getVectorZ(rJoints[count][JointType_KneeLeft], rJoints[count][JointType_AnkleLeft]);

	DrawLine(rJoints[count][JointType_SpineBase], rJoints[count][JointType_HipRight]);
	//X1[20] = getVectorX(rJoints[count][JointType_SpineBase], rJoints[count][JointType_HipRight]);
	//Y1[20] = getVectorY(rJoints[count][JointType_SpineBase], rJoints[count][JointType_HipRight]);
	//Z1[20] = getVectorZ(rJoints[count][JointType_SpineBase], rJoints[count][JointType_HipRight]);

	DrawLine(rJoints[count][JointType_HipRight], rJoints[count][JointType_KneeRight]);
	//X1[21] = getVectorX(rJoints[count][JointType_HipRight], rJoints[count][JointType_KneeRight]);
	//Y1[21] = getVectorY(rJoints[count][JointType_HipRight], rJoints[count][JointType_KneeRight]);
	//Z1[21] = getVectorZ(rJoints[count][JointType_HipRight], rJoints[count][JointType_KneeRight]);

	DrawLine(rJoints[count][JointType_KneeRight], rJoints[count][JointType_AnkleRight]);
	//X1[22] = getVectorX(rJoints[count][JointType_KneeRight], rJoints[count][JointType_AnkleRight]);
	//Y1[22] = getVectorY(rJoints[count][JointType_KneeRight], rJoints[count][JointType_AnkleRight]);
	//Z1[22] = getVectorZ(rJoints[count][JointType_KneeRight], rJoints[count][JointType_AnkleRight]);

	glEnd();
	//左右手關節角度計算
	CountRightH();
	CountLeftH();
	//送出命令
	if (connected)
		SendComend();
}

void display()
{

	// clear previous screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// draw points
	glPointSize(1.0f);

	// for each body
	for (int i = 0; i < iBodyCount; ++i)
	{
		IBody* pBody = aBody[i];
		if (pBody == nullptr)
			continue;

		// check if is tracked
		BOOLEAN bTracked = false;
		if ((pBody->get_IsTracked(&bTracked) == S_OK) && bTracked)
		{
			// get joint position
			Joint aJoints[JointType::JointType_Count];
			if (pBody->GetJoints(JointType::JointType_Count, aJoints) == S_OK)
			{
				if (pBody->GetJoints(JointType::JointType_Count, aJoints) == S_OK)
				{
					if ((recordFlag % 2) != 0) {	//是否開始錄影
						pBody->GetJoints(JointType::JointType_Count, rJoints[rCount]);
						rCount++;
					}
					else if ((replayFlag % 2) != 0) {	//是否播放錄影
						for (int i = 0; i < rCount - 1; ++i){
							Rec(i);
						}
						--replayFlag;
					}
					else if ((captureFlag % 2) != 0) {	//是否擷取畫面
						pBody->GetJoints(JointType::JointType_Count, rJoints[rCount]);
						rCount++;
						captureFlag--;
					}
				}
				if (recordFlag % 2 != 0) {
					selectFont(64, ANSI_CHARSET, "Comic Sans MS");
					glColor3f(1.0f, 0.0f, 0.0f);
					glRasterPos2f(0.0f, 0.0f);
					drawString("Record");
				}

				glLineWidth(5.0f);
				glBegin(GL_LINES);
				glColor3f(1, 0, 0);
				DrawLine(aJoints[JointType_SpineBase], aJoints[JointType_SpineMid]);	//畫出目前骨架並且儲存座標
				X2[0] = getVectorX(aJoints[JointType_SpineBase], aJoints[JointType_SpineMid]);
				Y2[0] = getVectorY(aJoints[JointType_SpineBase], aJoints[JointType_SpineMid]);
				Z2[0] = getVectorZ(aJoints[JointType_SpineBase], aJoints[JointType_SpineMid]);
				
				DrawLine(aJoints[JointType_SpineMid], aJoints[JointType_SpineShoulder]);
				X2[1] = getVectorX(aJoints[JointType_SpineMid], aJoints[JointType_SpineShoulder]);
				Y2[1] = getVectorY(aJoints[JointType_SpineMid], aJoints[JointType_SpineShoulder]);
				Z2[1] = getVectorZ(aJoints[JointType_SpineMid], aJoints[JointType_SpineShoulder]);

				DrawLine(aJoints[JointType_SpineShoulder], aJoints[JointType_Neck]);
				X2[2] = getVectorX(aJoints[JointType_SpineShoulder], aJoints[JointType_Neck]);
				Y2[2] = getVectorY(aJoints[JointType_SpineShoulder], aJoints[JointType_Neck]);
				Z2[2] = getVectorZ(aJoints[JointType_SpineShoulder], aJoints[JointType_Neck]);

				DrawLine(aJoints[JointType_Neck], aJoints[JointType_Head]);
				X2[3] = getVectorX(aJoints[JointType_Neck], aJoints[JointType_Head]);
				Y2[3] = getVectorY(aJoints[JointType_Neck], aJoints[JointType_Head]);
				Z2[3] = getVectorZ(aJoints[JointType_Neck], aJoints[JointType_Head]);

				DrawLine(aJoints[JointType_SpineShoulder], aJoints[JointType_ShoulderLeft]);
				X2[4] = getVectorX(aJoints[JointType_SpineShoulder], aJoints[JointType_ShoulderLeft]);
				Y2[4] = getVectorY(aJoints[JointType_SpineShoulder], aJoints[JointType_ShoulderLeft]);
				Z2[4] = getVectorZ(aJoints[JointType_SpineShoulder], aJoints[JointType_ShoulderLeft]);

				DrawLine(aJoints[JointType_ShoulderLeft], aJoints[JointType_ElbowLeft]);
				X2[5] = getVectorX(aJoints[JointType_ShoulderLeft], aJoints[JointType_ElbowLeft]);
				Y2[5] = getVectorY(aJoints[JointType_ShoulderLeft], aJoints[JointType_ElbowLeft]);
				Z2[5] = getVectorZ(aJoints[JointType_ShoulderLeft], aJoints[JointType_ElbowLeft]);

				DrawLine(aJoints[JointType_ElbowLeft], aJoints[JointType_WristLeft]);
				X2[6] = getVectorX(aJoints[JointType_ElbowLeft], aJoints[JointType_WristLeft]);
				Y2[6] = getVectorY(aJoints[JointType_ElbowLeft], aJoints[JointType_WristLeft]);
				Z2[6] = getVectorZ(aJoints[JointType_ElbowLeft], aJoints[JointType_WristLeft]);

				DrawLine(aJoints[JointType_WristLeft], aJoints[JointType_HandLeft]);
				X2[7] = getVectorX(aJoints[JointType_WristLeft], aJoints[JointType_HandLeft]);
				Y2[7] = getVectorY(aJoints[JointType_WristLeft], aJoints[JointType_HandLeft]);
				Z2[7] = getVectorZ(aJoints[JointType_WristLeft], aJoints[JointType_HandLeft]);
				DrawLine(aJoints[JointType_HandLeft], aJoints[JointType_HandTipLeft]);

				DrawLine(aJoints[JointType_SpineShoulder], aJoints[JointType_ShoulderRight]);
				X2[10] = getVectorX(aJoints[JointType_SpineShoulder], aJoints[JointType_ShoulderRight]);
				Y2[10] = getVectorY(aJoints[JointType_SpineShoulder], aJoints[JointType_ShoulderRight]);
				Z2[10] = getVectorZ(aJoints[JointType_SpineShoulder], aJoints[JointType_ShoulderRight]);

				DrawLine(aJoints[JointType_ShoulderRight], aJoints[JointType_ElbowRight]);
				X2[11] = getVectorX(aJoints[JointType_ShoulderRight], aJoints[JointType_ElbowRight]);
				Y2[11] = getVectorY(aJoints[JointType_ShoulderRight], aJoints[JointType_ElbowRight]);
				Z2[11] = getVectorZ(aJoints[JointType_ShoulderRight], aJoints[JointType_ElbowRight]);

				DrawLine(aJoints[JointType_ElbowRight], aJoints[JointType_WristRight]);
				X2[12] = getVectorX(aJoints[JointType_ElbowRight], aJoints[JointType_WristRight]);
				Y2[12] = getVectorY(aJoints[JointType_ElbowRight], aJoints[JointType_WristRight]);
				Z2[12] = getVectorZ(aJoints[JointType_ElbowRight], aJoints[JointType_WristRight]);

				DrawLine(aJoints[JointType_WristRight], aJoints[JointType_HandRight]);
				X2[13] = getVectorX(aJoints[JointType_WristRight], aJoints[JointType_HandRight]);
				Y2[13] = getVectorY(aJoints[JointType_WristRight], aJoints[JointType_HandRight]);
				Z2[13] = getVectorZ(aJoints[JointType_WristRight], aJoints[JointType_HandRight]);

				DrawLine(aJoints[JointType_HandRight], aJoints[JointType_HandTipRight]);

				DrawLine(aJoints[JointType_SpineBase], aJoints[JointType_HipLeft]);
				//X2[16] = getVectorX(aJoints[JointType_SpineBase], aJoints[JointType_HipLeft]);
				//Y2[16] = getVectorY(aJoints[JointType_SpineBase], aJoints[JointType_HipLeft]);
				//Z2[16] = getVectorZ(aJoints[JointType_SpineBase], aJoints[JointType_HipLeft]);

				DrawLine(aJoints[JointType_HipLeft], aJoints[JointType_KneeLeft]);
				//X2[17] = getVectorX(aJoints[JointType_HipLeft], aJoints[JointType_KneeLeft]);
				//Y2[17] = getVectorY(aJoints[JointType_HipLeft], aJoints[JointType_KneeLeft]);
				//Z2[17] = getVectorZ(aJoints[JointType_HipLeft], aJoints[JointType_KneeLeft]);

				DrawLine(aJoints[JointType_KneeLeft], aJoints[JointType_AnkleLeft]);
				//X2[18] = getVectorX(aJoints[JointType_KneeLeft], aJoints[JointType_AnkleLeft]);
				//Y2[18] = getVectorY(aJoints[JointType_KneeLeft], aJoints[JointType_AnkleLeft]);
				//Z2[18] = getVectorZ(aJoints[JointType_KneeLeft], aJoints[JointType_AnkleLeft]);

				DrawLine(aJoints[JointType_SpineBase], aJoints[JointType_HipRight]);
				//X2[20] = getVectorX(aJoints[JointType_SpineBase], aJoints[JointType_HipRight]);
				//Y2[20] = getVectorY(aJoints[JointType_SpineBase], aJoints[JointType_HipRight]);
				//Z2[20] = getVectorZ(aJoints[JointType_SpineBase], aJoints[JointType_HipRight]);

				DrawLine(aJoints[JointType_HipRight], aJoints[JointType_KneeRight]);
				//X2[21] = getVectorX(aJoints[JointType_HipRight], aJoints[JointType_KneeRight]);
				//Y2[21] = getVectorY(aJoints[JointType_HipRight], aJoints[JointType_KneeRight]);
				//Z2[21] = getVectorZ(aJoints[JointType_HipRight], aJoints[JointType_KneeRight]);

				DrawLine(aJoints[JointType_KneeRight], aJoints[JointType_AnkleRight]);
				//X2[22] = getVectorX(aJoints[JointType_KneeRight], aJoints[JointType_AnkleRight]);
				//Y2[22] = getVectorY(aJoints[JointType_KneeRight], aJoints[JointType_AnkleRight]);
				//Z2[22] = getVectorZ(aJoints[JointType_KneeRight], aJoints[JointType_AnkleRight]);

				glEnd();

				CountRightH();
				CountLeftH();
				if (connected) SendComend();

//******************************比對**********************************************
				if ((checkFlag % 2) != 0) {	
					Sleep(setting.gameSpeed);
					drawBody(checkCount);
					
					if (getVResult(X1, Y1, Z1, X2, Y2, Z2) == true) {
						passCount++;
						if (setting.gameMode == TRUE) {
							PlaySound(L"D:\\correct.wav", NULL, SND_FILENAME | SND_ASYNC);
							checkCount++;
						}
					}
					if(setting.gameMode == false) checkCount++;
					if (checkCount >= rCount) {	//判斷是否結束
						checkFlag--;
						checkCount = 0;
						totalCount = rCount;
					}
					selectFont(32, ANSI_CHARSET, "Comic Sans MS");
					if (setting.gameMode == TRUE) {
						glColor3f(1.0f, 1.0f, 1.0f);
						glRasterPos2f(-0.3f, -0.1f);
						drawString("MODE : static");
					}
					else {
						glColor3f(1.0f, 1.0f, 1.0f);
						glRasterPos2f(-0.3f, -0.1f);
						drawString("MODE : conitnus");
					}

					if (setting.gameAccuracy == setting.accuracyLow) {
						glColor3f(1.0f, 1.0f, 1.0f);
						glRasterPos2f(-0.3f, -0.2f);
						drawString("ACCURACT : Easy");
					}
					else if (setting.gameAccuracy == setting.accuracyNormal) {
						glColor3f(1.0f, 1.0f, 1.0f);
						glRasterPos2f(-0.3f, -0.2f);
						drawString("ACCURACT : Normal");
					}
					else {
						glColor3f(1.0f, 1.0f, 1.0f);
						glRasterPos2f(-0.3f, -0.2f);
						drawString("ACCURACT : Hard");
					}

					if (setting.gameSpeed == setting.speedSlow) {
						glColor3f(1.0f, 1.0f, 1.0f);
						glRasterPos2f(-0.3f, -0.3f);
						drawString("SPEED : Slow");
					}
					else if (setting.gameSpeed == setting.speedNormal) {
						glColor3f(1.0f, 1.0f, 1.0f);
						glRasterPos2f(-0.3f, -0.3f);
						drawString("SPEED : Normal");
					}
					else {
						glColor3f(1.0f, 1.0f, 1.0f);
						glRasterPos2f(-0.3f, -0.3f);
						drawString("SPEED : High");
					}

					selectFont(64, ANSI_CHARSET, "Comic Sans MS");
					glColor3f(1.0f, 1.0f, 1.0f);
					glRasterPos2f(-0.3f, 0.0f);
					drawString("Score:  ");
					drawString(getScore());
				}
			}
		}
	}

	// Coordinate


	// swap buffer
	glutSwapBuffers();
}


void idle()
{
	// Read body data
	IBodyFrame* pFrame = nullptr;
	if (pBodyFrameReader->AcquireLatestFrame(&pFrame) == S_OK)
	{
		// update body data
		pFrame->GetAndRefreshBodyData(iBodyCount, aBody);

		pFrame->Release();
		pFrame = nullptr;

		// redraw
		glutPostRedisplay();
	}
}

void reshape(int w, int h)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(40.0, (float)w / h, 0.01, 50.0);

	g_Camera.SetCamera();

	glViewport(0, 0, w, h);
}

void ExitFunction()
{
	delete[] aBody;

	// release frame reader
	pBodyFrameReader->Release();
	pBodyFrameReader = nullptr;

	// Close and Release Sensor
	pSensor->Close();
	pSensor->Release();
	pSensor = nullptr;
}

void start(int argc, char** argv)
{
	// 1. Sensor related code
	{
		if (GetDefaultKinectSensor(&pSensor) != S_OK)
		{
			cerr << "Get Sensor failed" << endl;
		}
		if (pSensor->Open() != S_OK)
		{
			cerr << "Can't open sensor" << endl;
		}
	}

	// 2. Body related code
	{
		// Get frame source
		IBodyFrameSource* pFrameSource = nullptr;
		if (pSensor->get_BodyFrameSource(&pFrameSource) != S_OK)
		{
			cerr << "Can't get body frame source" << endl;
		}

		// allocate body array
		if (pFrameSource->get_BodyCount(&iBodyCount) != S_OK)
		{
			cerr << "Can't get body count" << endl;
		}
		aBody = new IBody*[iBodyCount];
		for (int i = 0; i < iBodyCount; ++i)
			aBody[i] = nullptr;

		// get frame reader
		if (pFrameSource->OpenReader(&pBodyFrameReader) != S_OK)
		{
			cerr << "Can't get body frame reader" << endl;		}

		// release Frame source
		pFrameSource->Release();
		pFrameSource = nullptr;
	}

	// 5. initial glut
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB);

	glutInitWindowSize(640, 480);
	glutCreateWindow("動作畫面");

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);

	// default camera
	g_Camera.vCenter = Vector3(0.0, 0.0, 1);
	g_Camera.vPosition = Vector3(0.0, 0.0, -2.0);
	g_Camera.vUpper = Vector3(0.0, 1.0, 0.0);
	// register glut callback functions
	glutDisplayFunc(display);
	glutIdleFunc(idle);
	glutReshapeFunc(reshape);

	// STD Exit function
	atexit(ExitFunction);

	// start
	glutMainLoop();
}



// 如果將最小化按鈕加入您的對話方塊，您需要下列的程式碼，
// 以便繪製圖示。對於使用文件/檢視模式的 MFC 應用程式，
// 框架會自動完成此作業。

void CMFC_TESTDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 繪製的裝置內容

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 將圖示置中於用戶端矩形
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 描繪圖示
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
	start(NULL, NULL);

}



// 當使用者拖曳最小化視窗時，
// 系統呼叫這個功能取得游標顯示。
HCURSOR CMFC_TESTDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CMFC_TESTDlg::btn_record()	//錄製動作按鈕
{
	recordFlag++;
	if ((recordFlag % 2) != 0) SetDlgItemText(IDC_STATIC, L"錄製中");
	else SetDlgItemText(IDC_STATIC, L"");
	// TODO:  在此加入控制項告知處理常式程式碼
}


void CMFC_TESTDlg::btn_play()	//撥放暫存資料按鈕
{
	if((recordFlag % 2) == 0)replayFlag++;
	else AfxMessageBox(_T("正在錄影中"));
	if (rCount == 0) AfxMessageBox(_T("無錄影資料"));
	// TODO:  在此加入控制項告知處理常式程式碼
}


void CMFC_TESTDlg::OnEnChangeEdit1()
{
	// TODO:  如果這是 RICHEDIT 控制項，控制項將不會
	// 傳送此告知，除非您覆寫 CDialogEx::OnInitDialog()
	// 函式和呼叫 CRichEditCtrl().SetEventMask()
	// 讓具有 ENM_CHANGE 旗標 ORed 加入遮罩。
	
	
	// TODO:  在此加入控制項告知處理常式程式碼
}


void CMFC_TESTDlg::btn_clear()	//清除暫存資料按鈕
{
	AfxMessageBox(_T("已清除錄影資料"));
	rCount = 0;
	// TODO:  在此加入控制項告知處理常式程式碼
}


void CMFC_TESTDlg::OnBnClickedCancel()
{
	// TODO:  在此加入控制項告知處理常式程式碼
	dynamicFlag++;
	//CDialogEx::OnCancel();
}


void CMFC_TESTDlg::OnBnClickedOk()
{
	// TODO:  在此加入控制項告知處理常式程式碼
	setting.DoModal();
}


void CMFC_TESTDlg::btn_comparsion()	//開始比對按鈕
{
	if (rCount == 0) AfxMessageBox(_T("無錄影資料"));
	else {
		//replayFlag++;
		checkFlag++;
		checkCount = 0;
	}
	// TODO:  在此加入控制項告知處理常式程式碼

}


void CMFC_TESTDlg::btn_result()	//取得比對結果按鈕
{
	char temp1[100];
	_itoa_s(passCount, temp1, 10);
	CString csStr1(temp1);
	SetDlgItemText(IDC_STATIC_cCount, csStr1);
	char temp2[100];
	_itoa_s(rCount, temp2, 10);
	CString csStr2(temp2);
	SetDlgItemText(IDC_STATIC_rCount, csStr2);
	char temp3[100];
	int tempScore;
	tempScore = ((double)passCount / (double)rCount) * 100;
	CString str;
	str.Format(_T("%d"), tempScore);
	_itoa_s(tempScore, temp3, 10);
	CString csStr3(temp3);
	SetDlgItemText(IDC_STATIC_SCORE, csStr3);
	GetDlgItem(IDC_SCORE)->SetWindowTextW(str);
	passCount = 0;
	// TODO:  在此加入控制項告知處理常式程式碼
}


void CMFC_TESTDlg::btn_store()	//儲存資料按鈕
{
	CStdioFile file;
	CString filepath;
	TCHAR szFilter[] = _T("文本文件(*.txt)|*.txt|所有文件(*.*)|*.*||");
	CFileDialog fileDlg(FALSE, _T("txt"), _T("Record.txt"));

	if (rCount == 0) AfxMessageBox(_T("無錄影資料"));
	else {
		if (IDOK == fileDlg.DoModal())
		{
			filepath = fileDlg.GetPathName();
			file.Open(filepath, CFile::modeCreate | CFile::modeWrite | CFile::typeText);
			CString str;
			for (int i = 0; i < 40000; i++)
			{
				if ((rJoints[i][JointType::JointType_Count].Position.X == 0) || (rJoints[i][JointType::JointType_Count].Position.Y == 0)
					|| (rJoints[i][JointType::JointType_Count].Position.Z == 0)) break;

				str.Format(_T("%f \n %f \n %f \n"), rJoints[i][JointType_SpineBase].Position.X, rJoints[i][JointType_SpineBase].Position.Y, rJoints[i][JointType_SpineBase].Position.Z);
				file.WriteString(str);

				str.Format(_T("%f \n %f \n %f \n"), rJoints[i][JointType_SpineMid].Position.X, rJoints[i][JointType_SpineMid].Position.Y, rJoints[i][JointType_SpineMid].Position.Z);
				file.WriteString(str);

				str.Format(_T("%f \n %f \n %f \n"), rJoints[i][JointType_SpineShoulder].Position.X, rJoints[i][JointType_SpineShoulder].Position.Y, rJoints[i][JointType_SpineShoulder].Position.Z);
				file.WriteString(str);

				str.Format(_T("%f \n %f \n %f \n"), rJoints[i][JointType_Neck].Position.X, rJoints[i][JointType_Neck].Position.Y, rJoints[i][JointType_Neck].Position.Z);
				file.WriteString(str);

				str.Format(_T("%f \n %f \n %f \n"), rJoints[i][JointType_Head].Position.X, rJoints[i][JointType_Head].Position.Y, rJoints[i][JointType_Head].Position.Z);
				file.WriteString(str);

				str.Format(_T("%f \n %f \n %f \n"), rJoints[i][JointType_ShoulderLeft].Position.X, rJoints[i][JointType_ShoulderLeft].Position.Y, rJoints[i][JointType_ShoulderLeft].Position.Z);
				file.WriteString(str);

				str.Format(_T("%f \n %f \n %f \n"), rJoints[i][JointType_ElbowLeft].Position.X, rJoints[i][JointType_ElbowLeft].Position.Y, rJoints[i][JointType_ElbowLeft].Position.Z);
				file.WriteString(str);

				str.Format(_T("%f \n %f \n %f \n"), rJoints[i][JointType_WristLeft].Position.X, rJoints[i][JointType_WristLeft].Position.Y, rJoints[i][JointType_WristLeft].Position.Z);
				file.WriteString(str);

				str.Format(_T("%f \n %f \n %f \n"), rJoints[i][JointType_HandLeft].Position.X, rJoints[i][JointType_HandLeft].Position.Y, rJoints[i][JointType_HandLeft].Position.Z);
				file.WriteString(str);

				str.Format(_T("%f \n %f \n %f \n"), rJoints[i][JointType_ShoulderRight].Position.X, rJoints[i][JointType_ShoulderRight].Position.Y, rJoints[i][JointType_ShoulderRight].Position.Z);
				file.WriteString(str);

				str.Format(_T("%f \n %f \n %f \n"), rJoints[i][JointType_ElbowRight].Position.X, rJoints[i][JointType_ElbowRight].Position.Y, rJoints[i][JointType_ElbowRight].Position.Z);
				file.WriteString(str);

				str.Format(_T("%f \n %f \n %f \n"), rJoints[i][JointType_WristRight].Position.X, rJoints[i][JointType_WristRight].Position.Y, rJoints[i][JointType_WristRight].Position.Z);
				file.WriteString(str);

				str.Format(_T("%f \n %f \n %f \n"), rJoints[i][JointType_HandRight].Position.X, rJoints[i][JointType_HandRight].Position.Y, rJoints[i][JointType_HandRight].Position.Z);
				file.WriteString(str);

				str.Format(_T("%f \n %f \n %f \n"), rJoints[i][JointType_HipLeft].Position.X, rJoints[i][JointType_HipLeft].Position.Y, rJoints[i][JointType_HipLeft].Position.Z);
				file.WriteString(str);

				str.Format(_T("%f \n %f \n %f \n"), rJoints[i][JointType_KneeLeft].Position.X, rJoints[i][JointType_KneeLeft].Position.Y, rJoints[i][JointType_KneeLeft].Position.Z);
				file.WriteString(str);

				str.Format(_T("%f \n %f \n %f \n"), rJoints[i][JointType_AnkleLeft].Position.X, rJoints[i][JointType_AnkleLeft].Position.Y, rJoints[i][JointType_AnkleLeft].Position.Z);
				file.WriteString(str);

				str.Format(_T("%f \n %f \n %f \n"), rJoints[i][JointType_HipRight].Position.X, rJoints[i][JointType_HipRight].Position.Y, rJoints[i][JointType_HipRight].Position.Z);
				file.WriteString(str);

				str.Format(_T("%f \n %f \n %f \n"), rJoints[i][JointType_KneeRight].Position.X, rJoints[i][JointType_KneeRight].Position.Y, rJoints[i][JointType_KneeRight].Position.Z);
				file.WriteString(str);

				str.Format(_T("%f \n %f \n %f \n"), rJoints[i][JointType_AnkleRight].Position.X, rJoints[i][JointType_AnkleRight].Position.Y, rJoints[i][JointType_AnkleRight].Position.Z);
				file.WriteString(str);
			}
			file.Close();
			AfxMessageBox(_T("儲存成功!!"));
		}
	}
	// TODO:  在此加入控制項告知處理常式程式碼
}


void CMFC_TESTDlg::OnFileOpen()
{
	CFileDialog fDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT);
	fDlg.m_ofn.lpstrTitle = L"Open file";
	fDlg.m_ofn.lpstrFilter = L"Text Files(*.txt)\0*.txt\0All Files(*.*)\0*.*\0\0";
	if (fDlg.DoModal() == IDOK)
	{
		m_strFilePathName = fDlg.GetPathName();		//路徑
	}
}

void CMFC_TESTDlg::btn_load()	//讀取儲存資料按鈕
{

	OnFileOpen();
	int controller = 0;//判斷哪一個關節
	int i = 0;
	double tempXYZ = 0;
	char inputData[100];
	CString cs;
	fstream file;
	file.open(m_strFilePathName, ios::in);
	if (!file) AfxMessageBox(_T("檔案開啟失敗"));
	else {
		while (file.getline(inputData, sizeof(inputData), '\n')) {
			cs = inputData;
			tempXYZ = _tstof(cs);
			switch (controller / 3)	//判斷是哪個關節
			{
			case 0:
				switch (controller % 3)	//判斷XYZ
				{
				case 0:
					rJoints[rCount][JointType_SpineBase].Position.X = tempXYZ;
					break;
				case 1:
					rJoints[rCount][JointType_SpineBase].Position.Y = tempXYZ;
					break;
				case 2:
					rJoints[rCount][JointType_SpineBase].Position.Z = tempXYZ;
					break;
				}

			case 1:
				switch (controller % 3)
				{
				case 0:
					rJoints[rCount][JointType_SpineMid].Position.X = tempXYZ;
					break;
				case 1:
					rJoints[rCount][JointType_SpineMid].Position.Y = tempXYZ;
					break;
				case 2:
					rJoints[rCount][JointType_SpineMid].Position.Z = tempXYZ;
					break;
				}
			case 2:
				switch (controller % 3)
				{
				case 0:
					rJoints[rCount][JointType_SpineShoulder].Position.X = tempXYZ;
					break;
				case 1:
					rJoints[rCount][JointType_SpineShoulder].Position.Y = tempXYZ;
					break;
				case 2:
					rJoints[rCount][JointType_SpineShoulder].Position.Z = tempXYZ;
					break;
				}
			case 3:
				switch (controller % 3)
				{
				case 0:
					rJoints[rCount][JointType_Neck].Position.X = tempXYZ;
					break;
				case 1:
					rJoints[rCount][JointType_Neck].Position.Y = tempXYZ;
					break;
				case 2:
					rJoints[rCount][JointType_Neck].Position.Z = tempXYZ;
					break;
				}
			case 4:
				switch (controller % 3)
				{
				case 0:
					rJoints[rCount][JointType_Head].Position.X = tempXYZ;
					break;
				case 1:
					rJoints[rCount][JointType_Head].Position.Y = tempXYZ;
					break;
				case 2:
					rJoints[rCount][JointType_Head].Position.Z = tempXYZ;
					break;
				}
			case 5:
				switch (controller % 3)
				{
				case 0:
					rJoints[rCount][JointType_ShoulderLeft].Position.X = tempXYZ;
					break;
				case 1:
					rJoints[rCount][JointType_ShoulderLeft].Position.Y = tempXYZ;
					break;
				case 2:
					rJoints[rCount][JointType_ShoulderLeft].Position.Z = tempXYZ;
					break;
				}
			case 6:
				switch (controller % 3)
				{
				case 0:
					rJoints[rCount][JointType_ElbowLeft].Position.X = tempXYZ;
					break;
				case 1:
					rJoints[rCount][JointType_ElbowLeft].Position.Y = tempXYZ;
					break;
				case 2:
					rJoints[rCount][JointType_ElbowLeft].Position.Z = tempXYZ;
					break;
				}
			case 7:
				switch (controller % 3)
				{
				case 0:
					rJoints[rCount][JointType_WristLeft].Position.X = tempXYZ;
					break;
				case 1:
					rJoints[rCount][JointType_WristLeft].Position.Y = tempXYZ;
					break;
				case 2:
					rJoints[rCount][JointType_WristLeft].Position.Z = tempXYZ;
					break;
				}
			case 8:
				switch (controller % 3)
				{
				case 0:
					rJoints[rCount][JointType_HandLeft].Position.X = tempXYZ;
					break;
				case 1:
					rJoints[rCount][JointType_HandLeft].Position.Y = tempXYZ;
					break;
				case 2:
					rJoints[rCount][JointType_HandLeft].Position.Z = tempXYZ;
					break;
				}
			case 9:
				switch (controller % 3)
				{
				case 0:
					rJoints[rCount][JointType_ShoulderRight].Position.X = tempXYZ;
					break;
				case 1:
					rJoints[rCount][JointType_ShoulderRight].Position.Y = tempXYZ;
					break;
				case 2:
					rJoints[rCount][JointType_ShoulderRight].Position.Z = tempXYZ;
					break;
				}
			case 10:
				switch (controller % 3)
				{
				case 0:
					rJoints[rCount][JointType_ElbowRight].Position.X = tempXYZ;
					break;
				case 1:
					rJoints[rCount][JointType_ElbowRight].Position.Y = tempXYZ;
					break;
				case 2:
					rJoints[rCount][JointType_ElbowRight].Position.Z = tempXYZ;
					break;
				}
			case 11:
				switch (controller % 3)
				{
				case 0:
					rJoints[rCount][JointType_WristRight].Position.X = tempXYZ;
					break;
				case 1:
					rJoints[rCount][JointType_WristRight].Position.Y = tempXYZ;
					break;
				case 2:
					rJoints[rCount][JointType_WristRight].Position.Z = tempXYZ;
					break;
				}
			case 12:
				switch (controller % 3)
				{
				case 0:
					rJoints[rCount][JointType_HandRight].Position.X = tempXYZ;
					break;
				case 1:
					rJoints[rCount][JointType_HandRight].Position.Y = tempXYZ;
					break;
				case 2:
					rJoints[rCount][JointType_HandRight].Position.Z = tempXYZ;
					break;
				}
			case 13:
				switch (controller % 3)
				{
				case 0:
					rJoints[rCount][JointType_HipLeft].Position.X = tempXYZ;
					break;
				case 1:
					rJoints[rCount][JointType_HipLeft].Position.Y = tempXYZ;
					break;
				case 2:
					rJoints[rCount][JointType_HipLeft].Position.Z = tempXYZ;
					break;
				}
			case 14:
				switch (controller % 3)
				{
				case 0:
					rJoints[rCount][JointType_KneeLeft].Position.X = tempXYZ;
					break;
				case 1:
					rJoints[rCount][JointType_KneeLeft].Position.Y = tempXYZ;
					break;
				case 2:
					rJoints[rCount][JointType_KneeLeft].Position.Z = tempXYZ;
					break;
				}
			case 15:
				switch (controller % 3)
				{
				case 0:
					rJoints[rCount][JointType_AnkleLeft].Position.X = tempXYZ;
					break;
				case 1:
					rJoints[rCount][JointType_AnkleLeft].Position.Y = tempXYZ;
					break;
				case 2:
					rJoints[rCount][JointType_AnkleLeft].Position.Z = tempXYZ;
					break;
				}
			case 16:
				switch (controller % 3)
				{
				case 0:
					rJoints[rCount][JointType_HipRight].Position.X = tempXYZ;
					break;
				case 1:
					rJoints[rCount][JointType_HipRight].Position.Y = tempXYZ;
					break;
				case 2:
					rJoints[rCount][JointType_HipRight].Position.Z = tempXYZ;
					break;
				}
			case 17:
				switch (controller % 3)
				{
				case 0:
					rJoints[rCount][JointType_KneeRight].Position.X = tempXYZ;
					break;
				case 1:
					rJoints[rCount][JointType_KneeRight].Position.Y = tempXYZ;
					break;
				case 2:
					rJoints[rCount][JointType_KneeRight].Position.Z = tempXYZ;
					break;
				}
			case 18:
				switch (controller % 3)
				{
				case 0:
					rJoints[rCount][JointType_AnkleRight].Position.X = tempXYZ;
					break;
				case 1:
					rJoints[rCount][JointType_AnkleRight].Position.Y = tempXYZ;
					break;
				case 2:
					rJoints[rCount][JointType_AnkleRight].Position.Z = tempXYZ;
					break;
				}
			default:
				break;
			}

			controller++;

			if ((controller % 57) == 0 && (controller != 0)) { //完成一張骨架後歸零
				rCount++;
				controller = 0;
			}
		}
		AfxMessageBox(_T("檔案讀取成功"));
	}

	file.close();
	fstream testFile;
	testFile.open("Load_Record.txt", ios::out);
	for (int j = 0; j < 40000; j++) {
		if ((rJoints[j][JointType::JointType_Count].Position.X == 0) && (rJoints[j][JointType::JointType_Count].Position.Y == 0)
			&& (rJoints[j][JointType::JointType_Count].Position.Z == 0)) break;

		testFile << rJoints[j][JointType_SpineBase].Position.X << endl;
		testFile << rJoints[j][JointType_SpineBase].Position.Y << endl;
		testFile << rJoints[j][JointType_SpineBase].Position.Z << endl;

		testFile << rJoints[j][JointType_SpineMid].Position.X << endl;
		testFile << rJoints[j][JointType_SpineMid].Position.Y << endl;
		testFile << rJoints[j][JointType_SpineMid].Position.Z << endl;

		testFile << rJoints[j][JointType_SpineShoulder].Position.X << endl;
		testFile << rJoints[j][JointType_SpineShoulder].Position.Y << endl;
		testFile << rJoints[j][JointType_SpineShoulder].Position.Z << endl;

		testFile << rJoints[j][JointType_Neck].Position.X << endl;
		testFile << rJoints[j][JointType_Neck].Position.Y << endl;
		testFile << rJoints[j][JointType_Neck].Position.Z << endl;

		testFile << rJoints[j][JointType_Head].Position.X << endl;
		testFile << rJoints[j][JointType_Head].Position.Y << endl;
		testFile << rJoints[j][JointType_Head].Position.Z << endl;

		testFile << rJoints[j][JointType_ShoulderLeft].Position.X << endl;
		testFile << rJoints[j][JointType_ShoulderLeft].Position.Y << endl;
		testFile << rJoints[j][JointType_ShoulderLeft].Position.Z << endl;

		testFile << rJoints[j][JointType_ElbowLeft].Position.X << endl;
		testFile << rJoints[j][JointType_ElbowLeft].Position.Y << endl;
		testFile << rJoints[j][JointType_ElbowLeft].Position.Z << endl;

		testFile << rJoints[j][JointType_WristLeft].Position.X << endl;
		testFile << rJoints[j][JointType_WristLeft].Position.Y << endl;
		testFile << rJoints[j][JointType_WristLeft].Position.Z << endl;

		testFile << rJoints[j][JointType_HandLeft].Position.X << endl;
		testFile << rJoints[j][JointType_HandLeft].Position.Y << endl;
		testFile << rJoints[j][JointType_HandLeft].Position.Z << endl;

		testFile << rJoints[j][JointType_ShoulderRight].Position.X << endl;
		testFile << rJoints[j][JointType_ShoulderRight].Position.Y << endl;
		testFile << rJoints[j][JointType_ShoulderRight].Position.Z << endl;

		testFile << rJoints[j][JointType_ElbowRight].Position.X << endl;
		testFile << rJoints[j][JointType_ElbowRight].Position.Y << endl;
		testFile << rJoints[j][JointType_ElbowRight].Position.Z << endl;

		testFile << rJoints[j][JointType_WristRight].Position.X << endl;
		testFile << rJoints[j][JointType_WristRight].Position.Y << endl;
		testFile << rJoints[j][JointType_WristRight].Position.Z << endl;

		testFile << rJoints[j][JointType_HandRight].Position.X << endl;
		testFile << rJoints[j][JointType_HandRight].Position.Y << endl;
		testFile << rJoints[j][JointType_HandRight].Position.Z << endl;

		testFile << rJoints[j][JointType_HipLeft].Position.X << endl;
		testFile << rJoints[j][JointType_HipLeft].Position.Y << endl;
		testFile << rJoints[j][JointType_HipLeft].Position.Z << endl;

		testFile << rJoints[j][JointType_KneeLeft].Position.X << endl;
		testFile << rJoints[j][JointType_KneeLeft].Position.Y << endl;
		testFile << rJoints[j][JointType_KneeLeft].Position.Z << endl;

		testFile << rJoints[j][JointType_AnkleLeft].Position.X << endl;
		testFile << rJoints[j][JointType_AnkleLeft].Position.Y << endl;
		testFile << rJoints[j][JointType_AnkleLeft].Position.Z << endl;

		testFile << rJoints[j][JointType_HipRight].Position.X << endl;
		testFile << rJoints[j][JointType_HipRight].Position.Y << endl;
		testFile << rJoints[j][JointType_HipRight].Position.Z << endl;

		testFile << rJoints[j][JointType_KneeRight].Position.X << endl;
		testFile << rJoints[j][JointType_KneeRight].Position.Y << endl;
		testFile << rJoints[j][JointType_KneeRight].Position.Z << endl;

		testFile << rJoints[j][JointType_AnkleRight].Position.X << endl;
		testFile << rJoints[j][JointType_AnkleRight].Position.Y << endl;
		testFile << rJoints[j][JointType_AnkleRight].Position.Z << endl;
	}
	testFile.close();
	// TODO:  在此加入控制項告知處理常式程式碼
	char szBuf[100];
	_itoa_s(rCount, szBuf, 10);
	CString csStr(szBuf);
	SetDlgItemText(IDC_STATIC_rCount, csStr);
}


void CMFC_TESTDlg::OnStnClickedStaticrcount()
{
	// TODO:  在此加入控制項告知處理常式程式碼

}


void CMFC_TESTDlg::btn_capture()
{
	captureFlag++;
	AfxMessageBox(_T("擷取畫面成功!!"));
	// TODO:  在此加入控制項告知處理常式程式碼
}


void CMFC_TESTDlg::OnTcnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO:  在此加入控制項告知處理常式程式碼
	*pResult = 0;
}


void CMFC_TESTDlg::btn_pass()
{
	checkCount++;
	// TODO:  在此加入控制項告知處理常式程式碼
}


