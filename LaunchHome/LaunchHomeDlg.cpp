
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

void InitButon(MyButton& btn, int IdHover, int IdNomal, int idDisable) {
	btn.SetHoverBitmap(IdHover);
	btn.SetNomalBitmap(IdNomal);
	btn.LoadBitmaps(IdNomal, IdNomal, 0, IDB_BITMAP3);
}

void CLaunchHomeDlg::SetStatusText(MyText *text, LPCWCHAR display,COLORREF color) {
	if (text->index==-1)
		text->index = ++index_line_status_text;
	text->SetTextColor(color);
	text->Create(display, WS_VISIBLE,
		CRect(anchor_statusText_Left,
			anchor_statusText_Top+text->height_line*text->index, 200, text->height_line), this);
	text->SetWindowPos(NULL, 0, 0, 205, text->height_line, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);
}

void CLaunchHomeDlg::SetInfoText(MyText * text, LPCWCHAR display, COLORREF color)
{
	if (text->index != -1)
	{
		text->SetTextColor(color);
		text->SetWindowTextW(display);
	}
	else
	{
		text->index = ++index_line_status_text;
		text->SetTextColor(color);
		text->Create(display, WS_VISIBLE,
			CRect(anchor_InfoText_Left,
				anchor_InfoText_Top + text->height_line*text->index, 200, text->height_line), this);
		text->SetWindowPos(NULL, 0, 0, 205, text->height_line, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);
	}
	
}

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
	DDX_Control(pDX, IDC_BUTTON7, btnMinisize);
	DDX_Control(pDX, IDC_BUTTON8, btnClose);
}

BEGIN_MESSAGE_MAP(CLaunchHomeDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_ERASEBKGND()
	ON_BN_CLICKED(IDC_BUTTON1, &CLaunchHomeDlg::StartGame)
	ON_BN_CLICKED(IDC_BUTTON8, &CLaunchHomeDlg::CloseApp)
	ON_BN_CLICKED(IDC_BUTTON7, &CLaunchHomeDlg::HideWinndow)
	ON_BN_CLICKED(IDC_BUTTON6, &CLaunchHomeDlg::ExitApp)
	ON_BN_CLICKED(IDC_BUTTON2, &CLaunchHomeDlg::OpenHomePage)
	ON_BN_CLICKED(IDC_BUTTON3, &CLaunchHomeDlg::OpenRegisterPage)
	ON_BN_CLICKED(IDC_BUTTON4, &CLaunchHomeDlg::OpenTokenPage)
	ON_BN_CLICKED(IDC_BUTTON5, &CLaunchHomeDlg::OpenGroupPage)
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

	//setting button
	InitButon(this->btnMinisize, IDB_BITMAP18, IDB_BITMAP19, IDB_BITMAP19);
	InitButon(this->btnClose, IDB_BITMAP16, IDB_BITMAP17, IDB_BITMAP16);
	InitButon(this->btnStart, IDB_BITMAP2, IDB_BITMAP4, IDB_BITMAP3);
	InitButon(this->btnHome, IDB_BITMAP5, IDB_BITMAP6, IDB_BITMAP5);
	InitButon(this->btnRegister, IDB_BITMAP8, IDB_BITMAP7, IDB_BITMAP8);
	InitButon(this->btnToken, IDB_BITMAP10, IDB_BITMAP9, IDB_BITMAP10);
	InitButon(this->btnGroup, IDB_BITMAP11, IDB_BITMAP12, IDB_BITMAP11);
	InitButon(this->btnExit, IDB_BITMAP14, IDB_BITMAP13, IDB_BITMAP14);

	//setting text
	this->textLoadInfor = new MyText();
	SetStatusText(this->textLoadInfor,L"Dowloading Path.info...", RGB(255, 3, 36));
	
	MyText *test = new MyText();
	SetStatusText(test, L"Updating Path.info...", RGB(0, 196, 20));
	
	MyText *test2 = new MyText();
	SetInfoText(test2, L"Copy right by Hong Phuc", RGB(0, 196, 20));
	
	//if finish dowload then allow start game
	bool flagDowloadVersionDone = true;
	if (flagDowloadVersionDone)
		this->btnStart.EnableWindow(true);


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

	//GetClientRect(&clientRect);
	//pDC->FillSolidRect(clientRect, RGB(255, 255, 255));  // paint background in magenta


	return FALSE;
}

void CLaunchHomeDlg::StartGame()
{
	OutputDebugString(L"\nlick start game button\n");
	
}

void CLaunchHomeDlg::CloseApp()
{
	// TODO: Add your control notification handler code here
	CDialog::OnCancel();
}

void CLaunchHomeDlg::HideWinndow()
{
	// TODO: Add your control notification handler code here
	this->ShowWindow(SW_MINIMIZE);
}

void CLaunchHomeDlg::ExitApp()
{
	// TODO: Add your control notification handler code here
	CDialog::OnCancel();
}

void CLaunchHomeDlg::OpenHomePage()
{
	// TODO: Add your control notification handler code here
	ShellExecute(0, 0, MySettings::Instance()->homeUrl, 0, 0, SW_SHOW);
}

void CLaunchHomeDlg::OpenRegisterPage()
{
	// TODO: Add your control notification handler code here
	ShellExecute(0, 0, MySettings::Instance()->registerUrl, 0, 0, SW_SHOW);
}

void CLaunchHomeDlg::OpenTokenPage()
{
	// TODO: Add your control notification handler code here
	ShellExecute(0, 0, MySettings::Instance()->tokenUrl, 0, 0, SW_SHOW);
}

void CLaunchHomeDlg::OpenGroupPage()
{
	// TODO: Add your control notification handler code here
	ShellExecute(0, 0, MySettings::Instance()->groupUrl, 0, 0, SW_SHOW);
}

