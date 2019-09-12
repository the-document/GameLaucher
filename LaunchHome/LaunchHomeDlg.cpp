
// LaunchHomeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LaunchHome.h"
#include "LaunchHomeDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CLaunchHomeDlg dialog



CLaunchHomeDlg::CLaunchHomeDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_LAUNCHHOME_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CLaunchHomeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_PROGRESS1, downloadProgess);
	DDX_Control(pDX, IDC_PROGRESS2, unpackProgess);
	DDX_Control(pDX, IDC_BUTTON1, btnStart);
	DDX_Control(pDX, IDC_BUTTON2, btnHome);
	DDX_Control(pDX, IDC_BUTTON3, btnRegister);
	DDX_Control(pDX, IDC_BUTTON4, btnToken);
	DDX_Control(pDX, IDC_BUTTON5, btnGroup);
	DDX_Control(pDX, IDC_BUTTON6, btnExit);
}

BEGIN_MESSAGE_MAP(CLaunchHomeDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDC_BUTTON1, &CLaunchHomeDlg::StartGame)
END_MESSAGE_MAP()


// CLaunchHomeDlg message handlers

BOOL CLaunchHomeDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	OutputDebugString(L"\ninit dialog\n");
	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	//make transparent
	SetWindowLong(m_hWnd, GWL_EXSTYLE, GetWindowLong(m_hWnd, GWL_EXSTYLE) ^ WS_EX_LAYERED);
	SetLayeredWindowAttributes(RGB(255, 255, 255), 0, LWA_COLORKEY);

	//setcolor progessbar
	SetWindowTheme(downloadProgess.m_hWnd, L"", L"");
	downloadProgess.SendMessage(PBM_SETBARCOLOR, 0, RGB(255, 210, 14));
	this->downloadProgess.SetPos(80);
	CString percent;
	percent += "80";
	this->downloadProgess.SetWindowTextW(percent);

	SetWindowTheme(unpackProgess.m_hWnd, L"", L"");
	unpackProgess.SendMessage(PBM_SETBARCOLOR, 0, RGB(255, 210, 14));
	this->unpackProgess.SetPos(20);

	//this->SetBackgroundImage(IDB_BITMAP1, BACKGR_TOPLEFT, TRUE);
	
	//setting start button
	this->btnStart.SetHoverBitmap(IDB_BITMAP2);
	this->btnStart.SetNomalBitmap(IDB_BITMAP4);
	this->btnStart.LoadBitmaps(IDB_BITMAP3, IDB_BITMAP2, IDB_BITMAP4, IDB_BITMAP3);
	this->btnStart.SetWindowPos(NULL, 0, 0, 170, 95, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);
	//this->btnStart.SubclassDlgItem(IDC_BUTTON1, this);
	
	//setting home button
	this->btnHome.SetHoverBitmap(IDB_BITMAP5);
	this->btnHome.SetNomalBitmap(IDB_BITMAP6);
	this->btnHome.LoadBitmaps(IDB_BITMAP6, 0, IDB_BITMAP6, 0);
	this->btnHome.SetWindowPos(NULL, 0, 0, 70, 27, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);

	//setting register button
	this->btnRegister.SetHoverBitmap(IDB_BITMAP8);
	this->btnRegister.SetNomalBitmap(IDB_BITMAP7);
	this->btnRegister.LoadBitmaps(IDB_BITMAP7, 0, IDB_BITMAP8, 0);
	this->btnRegister.SetWindowPos(NULL, 0, 0, 70, 26, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);

	//setting token button
	this->btnToken.SetHoverBitmap(IDB_BITMAP10);
	this->btnToken.SetNomalBitmap(IDB_BITMAP9);
	this->btnToken.LoadBitmaps(IDB_BITMAP9, 0, IDB_BITMAP10, 0);
	this->btnToken.SetWindowPos(NULL, 0, 0, 70, 26, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);

	//setting group button
	this->btnGroup.SetHoverBitmap(IDB_BITMAP11);
	this->btnGroup.SetNomalBitmap(IDB_BITMAP12);
	this->btnGroup.LoadBitmaps(IDB_BITMAP12, 0, IDB_BITMAP11, 0);
	this->btnGroup.SetWindowPos(NULL, 0, 0, 70, 26, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);

	//setting exit button
	this->btnExit.SetHoverBitmap(IDB_BITMAP14);
	this->btnExit.SetNomalBitmap(IDB_BITMAP13);
	this->btnExit.LoadBitmaps(IDB_BITMAP13, 0, IDB_BITMAP14, 0);
	this->btnExit.SetWindowPos(NULL, 0, 0, 70, 26, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);

	//if finish dowload then allow start game
	bool flagDowloadVersionDone = true;
	if (flagDowloadVersionDone)
		this->btnStart.EnableWindow(flagDowloadVersionDone);


	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CLaunchHomeDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CLaunchHomeDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL CLaunchHomeDlg::OnEraseBkgnd(CDC * pDC)
{
	CRect clientRect;

	GetClientRect(&clientRect);
	pDC->FillSolidRect(clientRect, RGB(255, 255, 255));  // paint background in magenta


	return FALSE;
}



void CLaunchHomeDlg::StartGame()
{
	OutputDebugString(L"\nlick start game button\n");
}
