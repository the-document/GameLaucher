
// LaunchHomeDlg.h : header file
//

#pragma once
#include"MyButton.h"
#include"MyText.h"
#include"MySettings.h"


// CLaunchHomeDlg dialog
class CLaunchHomeDlg : public CDialogEx
{
// Construction
public:
	CLaunchHomeDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_LAUNCHHOME_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	DECLARE_MESSAGE_MAP()

private:
	int anchor_statusText_Top = 410;
	int anchor_statusText_Left = 420;
	int index_line_status_text = -1;

	int anchor_InfoText_Top = 240;
	int anchor_InfoText_Left = 750;
	int index_line_InfoText = -1;

public:
	CProgressCtrl downloadProgess;
	CProgressCtrl unpackProgess;
	
	afx_msg void StartGame();

	MyButton btnStart;
	MyButton btnHome;
	MyButton btnRegister;
	MyButton btnToken;
	MyButton btnGroup;
	MyButton btnExit;

	MyText *textLoadInfor;
	MyButton btnMinisize;
	MyButton btnClose;
	afx_msg void CloseApp();
	afx_msg void HideWinndow();

	afx_msg void ExitApp();
	afx_msg void OpenHomePage();
	afx_msg void OpenRegisterPage();
	afx_msg void OpenTokenPage();
	afx_msg void OpenGroupPage();

	void SetStatusText(MyText *text,LPCWCHAR display, COLORREF color);
	void SetInfoText(MyText *text, LPCWCHAR display, COLORREF color);
};
