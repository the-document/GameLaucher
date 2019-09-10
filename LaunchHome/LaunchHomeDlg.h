
// LaunchHomeDlg.h : header file
//

#pragma once


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
	
public:

	CButton buttonStart;

	CProgressCtrl downloadProgess;
	CProgressCtrl unpackProgess;
	
};
