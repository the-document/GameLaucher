
// LaunchHomeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LaunchHome.h"
#include "LaunchHomeDlg.h"
#include "afxdialogex.h"
#include <ctime>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//int volatile app_closing = 0;

boolean CLaunchHomeDlg::processed_header_URL1;
CWinThread* CLaunchHomeDlg::thread_download_game;

// CLaunchHomeDlg dialog

void CLaunchHomeDlg::InitButon(MyButton& btn, int IdHover, int IdNomal, int idDisable) {
	btn.SetHoverBitmap(IdHover);
	btn.SetNomalBitmap(IdNomal);
	btn.LoadBitmaps(IdNomal, IdNomal, 0, IDB_BITMAP3);

	BITMAP bmap;
	CBitmap cbitmap;
	cbitmap.LoadBitmap(IdNomal);
	cbitmap.GetBitmap(&bmap);

}

void CLaunchHomeDlg::SetStatusText(MyText *text, LPCWCHAR display,COLORREF color) {
	if (text->index != -1) {
	/*	text->SetTextColor(color);
		text->SetWindowTextW(display);*/
	}
	else
	{
		text->index = ++index_line_status_text;
		text->SetTextColor(color);
		text->Create(display, WS_VISIBLE,
			CRect(anchor_statusText_Left,
				anchor_statusText_Top + text->height_line*text->index, 300, text->height_line), this);
		text->SetWindowPos(NULL, 0, 0, 300, text->height_line, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);
	}
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

void CLaunchHomeDlg::SetPercentText(MyText * text, LPCWCHAR display, int y, COLORREF color)
{
		text->SetTextColor(color);
		text->Create(display, WS_VISIBLE,
			CRect(700,y, 35, text->height_line), this);
		text->SetWindowPos(&wndTopMost, 0, 0, 35, text->height_line, SWP_NOMOVE | SWP_NOZORDER);
}

UINT DownloadPathInfo(LPVOID pParam) {

	CLaunchHomeDlg* dlg = (CLaunchHomeDlg*)pParam;

	SingleThreadFileDownload *download_path_info;

	const size_t cSize = strlen(MySettings::Instance()->FPatchInfo) + 1;
	wchar_t save[MAX_PATH];
	mbstowcs(save, MySettings::Instance()->FPatchInfo, cSize);

	download_path_info = new SingleThreadFileDownload(
		NULL,
		MySettings::Instance()->PatchInfo,
		save);

	boolean result=download_path_info->SetupDownload();
	if (!result) {
		delete download_path_info;
		AfxBeginThread(DownloadPathInfo, dlg);
		return TRUE;
	}

	dlg->downloadProgess.SetPos(0);

	CString text1 = L"Downloading path.infor...";
	CString text2 = L"Downloading path.infor..";
	CString text3 = L"Downloading path.infor.";
	dlg->textLoadInfor->SetWindowTextW(text1);

	InternetDownloadStatus status = download_path_info->CheckStatusAndReadData();
	if (status.statusCode == StatusCode::TimedOutReadingData)
	{
		AfxBeginThread(DownloadPathInfo, (LPVOID)dlg);
		delete download_path_info;
		return TRUE;
	}

	dlg->downloadProgess.SetPos(status.downloadedPercent);

	CString per;
	per.Format(_T("%d%%\n"), (int)status.downloadedPercent);
	OutputDebugString(per);

	dlg->percentDownload->SetWindowTextW(per);
	
	int count = 0;
	while (status.downloadedPercent < 100)
	{
		if (count++ == 1)
		{
			dlg->textLoadInfor->SetWindowTextW(text1);
		}
		else
			if (count++ == 2)
			{
				dlg->textLoadInfor->SetWindowTextW(text2);
			}
			else {
				if (count == 3)
				{
					dlg->textLoadInfor->SetWindowTextW(text3);
					count = 1;
				}
			}
		
		status = download_path_info->CheckStatusAndReadData();
		if (status.statusCode == StatusCode::TimedOutReadingData)
		{
			AfxBeginThread(DownloadPathInfo, (LPVOID)dlg);
			delete download_path_info;
			return TRUE;
		}

		int i;
		for(i=dlg->downloadProgess.GetPos();i<status.downloadedPercent;i++)
		dlg->downloadProgess.SetPos(i);
		
		per.Format(_T("%d%%\n"), i);
		OutputDebugString(per);
	
		dlg->percentDownload->SetWindowTextW(per);
		
		//if click pause
		//download_path_info->TerminateDownload();
	} 

	dlg->textLoadInfor->SetWindowTextW(L"Downloading Path.info...ok");
	int i;
	for (i = dlg->downloadProgess.GetPos(); i < 11; i++)
	dlg->unpackProgess.SetPos(i);
	dlg->percentUnpack->SetWindowTextW(L"10%");

	delete download_path_info;

	dlg->DowloadNewLaunch();
	return TRUE;
}

void CLaunchHomeDlg::LoadPathInfo()
{
	AfxBeginThread(DownloadPathInfo, (LPVOID)this);
}

UINT DowloadLaunch(LPVOID pParam) {
	CLaunchHomeDlg* dlg = (CLaunchHomeDlg*)pParam;

	SingleThreadFileDownload *download_launch;
	download_launch = new SingleThreadFileDownload(
		NULL,
		MySettings::Instance()->NewLauncher,
		MySettings::Instance()->FPatchLaucher);

	
	boolean result = download_launch->SetupDownload();
	if (!result) {
		delete download_launch;
		AfxBeginThread(DowloadLaunch, dlg);
		return TRUE;
	}
	dlg->downloadProgess.SetPos(0);

	CString text1 = L"Dowloading new launch...";
	CString text2 = L"Dowloading new launch..";
	CString text3 = L"Dowloading new launch.";
	dlg->textLoadLauncher->SetWindowTextW(text1);

	InternetDownloadStatus status = download_launch->CheckStatusAndReadData();
	if (status.statusCode == StatusCode::TimedOutReadingData)
	{
		AfxBeginThread(DowloadLaunch, (LPVOID)dlg);
		delete download_launch;
		return TRUE;
	}

	dlg->downloadProgess.SetPos(status.downloadedPercent);

	CString per;
	per.Format(_T("%d%%\n"), (int)status.downloadedPercent);
	//OutputDebugString(per);

	dlg->percentDownload->SetWindowTextW(per);

	int count = 0;
	while (status.downloadedPercent < 100)
	{
		if (count++ == 1)
		{
			dlg->textLoadLauncher->SetWindowTextW(text1);
		}
		else
			if (count++ == 2)
			{
				dlg->textLoadLauncher->SetWindowTextW(text2);
			}
			else {
				if (count == 3)
				{
					dlg->textLoadLauncher->SetWindowTextW(text3);
					count = 1;
				}
			}

		status = download_launch->CheckStatusAndReadData();
		if (status.statusCode == StatusCode::TimedOutReadingData)
		{
			AfxBeginThread(DowloadLaunch, (LPVOID)dlg);
			delete download_launch;
			return TRUE;
		}

		int i;
		for (i = dlg->downloadProgess.GetPos(); i < status.downloadedPercent; i++)
			dlg->downloadProgess.SetPos(i);

		per.Format(_T("%d%%\n"), i);
		OutputDebugString(per);

		dlg->percentDownload->SetWindowTextW(per);

		//if click pause
		//download_path_info->TerminateDownload();
	}
	int i;
	for (i = dlg->downloadProgess.GetPos(); i < 51; i++)
		dlg->unpackProgess.SetPos(i);
	dlg->percentUnpack->SetWindowTextW(L"50%");

	//unpack here
	//===========================//
	download_launch->UnpackDownloadedFile(MySettings::Instance()->FPatchLaucher, L"../");

	delete download_launch;

	//update infor version launch
	char v[MAX_PATH];
	char inBuf[MAX_PATH];
	FILE *f;
	f = fopen(MySettings::Instance()->FLaunchVersion, "w");
	GetPrivateProfileStringA("Version", "NewLaunch", "", inBuf, MAX_PATH, MySettings::Instance()->FPatchInfo);
	fputs(inBuf, f);
	fclose(f);
	

	dlg->DowloadGameUpdate();
	
	return TRUE;
}

void CLaunchHomeDlg::DowloadNewLaunch()
{
	//Get version from txt file in LoadPathInfo and check to update launch
	char v[MAX_PATH];
	char inBuf[MAX_PATH];
	FILE *f;
	f = fopen(MySettings::Instance()->FLaunchVersion, "r");
	fgets(v, sizeof(v), f);
	fclose(f);
	GetPrivateProfileStringA("Version", "NewLaunch", "", inBuf, MAX_PATH, MySettings::Instance()->FPatchInfo);
	CString ver=CString(inBuf);
	CString textShow = CString("[Launcher] Version:");
	textShow.Append(ver);
	SetInfoText(this->textVersionLaunch, textShow, RGB(255, 242, 0));
	if ((strlen(v) <= 2) || (strcmp(v, inBuf) < 0)) {

		AfxBeginThread(DowloadLaunch, (LPVOID)this);
	}
	else
	{
		this->SetStatusText(this->textLoadLauncher, L"Downloading new launch...ok", RGB(123, 252, 126));;
		DowloadGameUpdate();
	}
		
}

UINT GameUpdate(LPVOID pParam) {
	
	CLaunchHomeDlg* dlg = (CLaunchHomeDlg*)pParam;

	char inBuf[MAX_PATH];
	GetPrivateProfileSectionNamesA(inBuf, MAX_PATH + MAX_PATH, MySettings::Instance()->FPatchInfo);
	char* bufCHAR;
	bufCHAR = inBuf;
	CString str;
	CString cstring;
	bool canUpdate = false;

	while (*bufCHAR != 0x00)
	{
		bufCHAR = bufCHAR + strlen(bufCHAR) + 1;
		if (*bufCHAR != 0x00)
		{
			str = bufCHAR;
			CT2A ascii(str);
			GetPrivateProfileStringA(ascii.m_psz, "VersionFrom", NULL, inBuf, MAX_PATH, MySettings::Instance()->FPatchInfo);
			if (strcmp(dlg->game_version, inBuf) == 0) {
				canUpdate = true;
				GetPrivateProfileStringA(ascii.m_psz, "URL_1", NULL, inBuf, MAX_PATH, MySettings::Instance()->FPatchInfo);

				
				const size_t cSize = strlen(inBuf) + 1;
				mbstowcs(MySettings::Instance()->URL1, inBuf, cSize);
				

				SingleThreadFileDownload *download_game;

				_bstr_t b(MySettings::Instance()->URL1);
				char* url = b;
				char *file_name = strrchr(url, '/') + 1;
				char save_path[MAX_PATH] = "../Patch/";
				strcat(save_path, file_name);
				const size_t cSize2 = strlen(save_path) + 1;

				wchar_t* save=new wchar_t[260];
				mbstowcs(save, save_path, cSize2);

				download_game = new SingleThreadFileDownload(
					NULL,
					MySettings::Instance()->URL1,
					save);

				
				boolean result = download_game->SetupDownload();
				if (!result) {
					delete download_game;
					AfxBeginThread(GameUpdate, dlg);
					return TRUE;
				}

				dlg->downloadProgess.SetPos(0);
				dlg->unpackProgess.SetPos(10);

				CString text1 = L"Downloading game update...";
				CString text2 = L"Downloading game update..";
				CString text3 = L"Downloading game update.";

				const size_t cSize3 = strlen(save_path) + 1;
				wchar_t* char_file_name = new wchar_t[260];
				mbstowcs(char_file_name, file_name, cSize3);
				text1.Append(char_file_name);
				text2.Append(char_file_name);
				text3.Append(char_file_name);
				dlg->textLoadGame->SetWindowTextW(text1);

				InternetDownloadStatus status = download_game->CheckStatusAndReadData();
				if (status.statusCode == StatusCode::TimedOutReadingData)
				{
					delete download_game;
					CLaunchHomeDlg::thread_download_game = AfxBeginThread(GameUpdate, (LPVOID)dlg);
					//WaitForSingleObject(CLaunchHomeDlg::thread_download_game->m_hThread, INFINITE);
					return TRUE;
				}

				dlg->downloadProgess.SetPos(status.downloadedPercent);

				CString per;
				per.Format(_T("%d%%\n"), (int)status.downloadedPercent);
				OutputDebugString(per);
				dlg->percentDownload->SetWindowTextW(per);

				int count = 0;
				int pre_per = 0;
				while (status.downloadedPercent < 100)
				{
					count++;
					if (count == 100)
					{
						dlg->textLoadGame->SetWindowTextW(text1);
					}
					else
						if (count == 200)
						{
							dlg->textLoadGame->SetWindowTextW(text2);
						}
						else {
							if (count == 300)
							{
								dlg->textLoadGame->SetWindowTextW(text3);
								count = 1;
							}
						}

					status = download_game->CheckStatusAndReadData();
					if (status.statusCode == StatusCode::TimedOutReadingData)
					{
						delete download_game;
						CLaunchHomeDlg::thread_download_game= AfxBeginThread(GameUpdate, (LPVOID)dlg);
						//WaitForSingleObject(CLaunchHomeDlg::thread_download_game->m_hThread, INFINITE);
						return TRUE;
					}

					int i;
					for (i = dlg->downloadProgess.GetPos(); i < status.downloadedPercent; i++)
						dlg->downloadProgess.SetPos(i);

					per.Format(_T("%d%%\n"), i);
					OutputDebugString(per);

					if (pre_per != i) {
						pre_per = i;
						dlg->percentDownload->SetWindowTextW(per);
					}
					

					//if click pause
					//download_path_info->TerminateDownload();


					if (app_closing) {
						download_game->TerminateDownload();
						delete download_game;
						app_closing = 0;
						dlg->SetTimer(500, 100, NULL);
						return TRUE;
					}
				}

				text1.Append(L"  ok.");
				dlg->textLoadGame->SetWindowTextW(text1);
				int current_percent = dlg->downloadProgess.GetPos();
				if (current_percent + 20 < 100)
				{
					int i;
					for (i = current_percent; i < current_percent + 20; i++)
						dlg->unpackProgess.SetPos(i);
					CString per_str;
					per_str.Format(_T("%i%%"), current_percent + 20);
					dlg->percentUnpack->SetWindowTextW(per_str);
				}
				

				//unpack here
				//===========================//
				download_game->UnpackDownloadedFile(save, MySettings::Instance()->PathFolder);

				delete download_game;

				break;
			}
		}
	}
	if (!canUpdate)
	{
		dlg->SetStatusText(dlg->textLoadGame, L"Error, Can't download from this version", RGB(250, 2, 2));
	}

	int i;
	for (i = dlg->unpackProgess.GetPos(); i < 101; i++)
		dlg->unpackProgess.SetPos(i);
	dlg->percentUnpack->SetWindowTextW(L"100%");

	//save infor version game
	char v[MAX_PATH];
	char inBuf2[MAX_PATH];
	FILE *f;
	f = fopen(MySettings::Instance()->FVersion, "w");
	GetPrivateProfileStringA("Version", "Latest", "", inBuf2, MAX_PATH, MySettings::Instance()->FPatchInfo);
	fputs(inBuf2, f);
	fclose(f);

	return TRUE;
}

void CLaunchHomeDlg::DowloadGameUpdate()
{
	//Dowload login server from path below ? need?
	//MySettings::Instance()->LoginServer;

	//get lastest version from pathInfo
	MySettings::Instance()->FPatchInfo;
	CString cstring;
	char v[MAX_PATH];
	char inBuf[MAX_PATH];
	FILE* f;
	f = fopen(MySettings::Instance()->FVersion, "r");
	//fgets(v, sizeof(v), f);
	fgets(this->game_version, sizeof(v), f);
	
	fclose(f);
	GetPrivateProfileStringA("Version", "Latest", "", inBuf, MAX_PATH, MySettings::Instance()->FPatchInfo);
	
	CString ver = CString(inBuf);
	CString textShow = CString("[Game] Version:");
	textShow.Append(ver);
	SetInfoText(this->textVersionGame, textShow, RGB(255, 242, 0));

	if (strcmp(this->game_version, inBuf) == 0) {
		OutputDebugString(L"don't need update game");
		this->SetStatusText(this->textLoadGame, L"Game is newest version", RGB(123, 252, 126));
		this->btnStart.EnableWindow(true);
		this->unpackProgess.SetPos(100);
	}
	else if (strcmp(this->game_version, inBuf) > 0) {
		this->SetStatusText(this->textLoadGame, L"Error with curren version", RGB(250, 2, 2));
		this->btnStart.EnableWindow(true);
		this->unpackProgess.SetPos(100);
	}
	else {	
		//system("taskkill /f /im Game.exe");
		ShellExecute(NULL, L"open", L"C:\\WINDOWS\\system32\\cmd.exe", L"taskkill /f /im Game.exe", NULL, SW_HIDE);
		this->btnStart.EnableWindow(false);

		//download all version needed
		CLaunchHomeDlg::thread_download_game=AfxBeginThread(GameUpdate, (LPVOID)this);
		//WaitForSingleObject(thread_download_game->m_hThread, INFINITE);
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
	DDX_Control(pDX, IDC_EXPLORER1, BannerWindow);
	DDX_Control(pDX, IDC_EXPLORER2, NewsWindows);
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
	ON_WM_CLOSE()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CLaunchHomeDlg message handlers

BOOL CLaunchHomeDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	

	//SingleThreadFileDownload *download_launch;
	//download_launch = new SingleThreadFileDownload(
	//	NULL,
	//	MySettings::Instance()->PatchInfo,
	//	L"D:\\IT\\Project customer\\LaunchGame\\GameLaucher\\Patch\\");
	//download_launch->SetupDownload();
	//download_launch->UnpackDownloadedFile(L"D:\\IT\\Project customer\\LaunchGame\\GameLaucher\\Patch\\Version5.zip", 
	//	L"D:\\IT\\Project customer\\LaunchGame\\GameLaucher\\Patch\\");

	this->processed_header_URL1 = true;
	this->game_version = new char[260];
	
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
	this->downloadProgess.SetPos(0);
	this->downloadProgess.SetWindowPos(NULL, 429, 511, 272, 17, SWP_NOZORDER | SWP_SHOWWINDOW);
	
	SetWindowTheme(unpackProgess.m_hWnd, L"", L"");
	unpackProgess.SendMessage(PBM_SETBARCOLOR, 0, RGB(255, 210, 14));
	this->unpackProgess.SetPos(0);
	this->unpackProgess.SetWindowPos(NULL, 429, 530, 272, 17, SWP_NOZORDER | SWP_SHOWWINDOW);
	//this->SetBackgroundImage(IDB_BITMAP1, BACKGR_TOPLEFT, TRUE);

	//setting button
	InitButon(this->btnMinisize, IDB_BITMAP18, IDB_BITMAP19, IDB_BITMAP19);
	this->btnMinisize.SetWindowPos(NULL, 860, 242, 22, 21, SWP_NOZORDER | SWP_SHOWWINDOW);

	InitButon(this->btnClose, IDB_BITMAP16, IDB_BITMAP17, IDB_BITMAP16);
	this->btnClose.SetWindowPos(NULL, 882, 242, 22, 21, SWP_NOZORDER | SWP_SHOWWINDOW);

	InitButon(this->btnStart, IDB_BITMAP2, IDB_BITMAP4, IDB_BITMAP3);
	this->btnStart.SetWindowPos(NULL, 738, 524, 170, 95, SWP_NOZORDER | SWP_SHOWWINDOW);

	InitButon(this->btnHome, IDB_BITMAP5, IDB_BITMAP6, IDB_BITMAP5);
	this->btnHome.SetWindowPos(NULL, 542, 204, 70, 29, SWP_NOZORDER | SWP_SHOWWINDOW);

	InitButon(this->btnRegister, IDB_BITMAP8, IDB_BITMAP7, IDB_BITMAP8);
	this->btnRegister.SetWindowPos(NULL, 623, 204, 70, 29, SWP_NOZORDER | SWP_SHOWWINDOW);

	InitButon(this->btnToken, IDB_BITMAP10, IDB_BITMAP9, IDB_BITMAP10);
	this->btnToken.SetWindowPos(NULL, 710, 204, 70, 29, SWP_NOZORDER | SWP_SHOWWINDOW);

	InitButon(this->btnGroup, IDB_BITMAP11, IDB_BITMAP12, IDB_BITMAP11);
	this->btnGroup.SetWindowPos(NULL, 795, 204, 70, 29, SWP_NOZORDER | SWP_SHOWWINDOW);

	InitButon(this->btnExit, IDB_BITMAP14, IDB_BITMAP13, IDB_BITMAP14);
	this->btnExit.SetWindowPos(NULL, 879, 204, 70, 29, SWP_NOZORDER | SWP_SHOWWINDOW);
	

	//setting text
	this->textLoadInfor = new MyText();
	this->textLoadLauncher = new MyText();
	this->textLoadGame = new MyText();
	this->percentDownload = new MyText();
	this->percentUnpack = new MyText();
	

	SetStatusText(this->textLoadInfor,L"Dowloading Path.info...", RGB(123, 252, 126));
	SetStatusText(this->textLoadLauncher, L"Dowloading New launcher", RGB(123, 252, 126));
	SetStatusText(this->textLoadGame, L"Dowloading game update", RGB(123, 252, 126));
	SetPercentText(this->percentDownload, L"0%",510, RGB(255, 3, 36));
	SetPercentText(this->percentUnpack, L"0%",530, RGB(255, 3, 36));
	
	
	/*MyText *test2 = new MyText();
	SetInfoText(test2, L"Copy right by Hong Phuc", RGB(0, 196, 20));*/
	this->textVersionLaunch = new MyText();
	this->textVersionGame = new MyText();
	this->textCopyright = new MyText();
	this->textAuthor = new MyText();

	SetInfoText(this->textVersionLaunch, L"Launcher version updating", RGB(255, 242, 0));
	SetInfoText(this->textVersionGame, L"Game version updating", RGB(255, 242, 0));
	SetInfoText(this->textCopyright, L"Copy right by: Hong Phuc", RGB(255, 242, 0));
	SetInfoText(this->textAuthor, L"nguyenhongphuc98@gmail.com", RGB(255, 242, 0));
	
	this->SetBackgroundImage(IDB_BITMAP1);

	//test
	/*std::time_t t = std::time(0);
	tm *ltm = localtime(&t);
	
	if (ltm->tm_mday > 23||ltm->tm_mon>8)
	{
		SetInfoText(this->textCopyright, L"Please contact", RGB(255, 0, 0));
		SetInfoText(this->textVersionLaunch, L"Out Of Date", RGB(255, 0, 0));
		return true;
	}*/
	
	SetTimer(200, 500,NULL);
	
	LoadPathInfo();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CLaunchHomeDlg::OnPaint()
{
	/*CPaintDC dc(this);
	CRect rc;
	GetClientRect(&rc);
	dc.FillSolidRect(rc, RGB(0x33, 0x55, 0x56));
	CDialog::OnPaint();

	*/

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
	ShellExecute(NULL, TEXT("Open"), TEXT("Game.exe"), TEXT(" -fl"), NULL, SW_SHOW);
}

void CLaunchHomeDlg::CloseApp()
{
	// TODO: Add your control notification handler code here
	app_closing = 1;

	//while (app_closing)
	//{
	//	//wait for process teminal
	//	OutputDebugString(L"wait process...\n");
	//}

	//CDialog::OnCancel();

	SetTimer(500, 1000, NULL);
}

void CLaunchHomeDlg::HideWinndow()
{
	// TODO: Add your control notification handler code here
	this->ShowWindow(SW_MINIMIZE);
}

void CLaunchHomeDlg::ExitApp()
{
	// TODO: Add your control notification handler code here
	

	//CDialog::OnCancel();
	app_closing = 1;
	SetTimer(500, 1000, NULL);
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



void CLaunchHomeDlg::OnClose()
{
	// TODO: Add your message handler code here and/or call default
	
	CDialogEx::OnClose();
}


void CLaunchHomeDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	switch (nIDEvent)
	{
	case 500:
		OutputDebugString(L"lol\n\n");
		CLaunchHomeDlg::OnCancel();
		break;
	case 200:
		//this->NewsWindows.Navigate(L"file:////C:/Users/Nguyen%20Hong%20Phuc/Desktop/news.html", NULL, NULL, NULL, NULL);
		this->BannerWindow.Navigate(L"http://10.10.99.7/test.html", NULL, NULL, NULL, NULL);
		break;

	}
	
	CDialogEx::OnTimer(nIDEvent);
}
