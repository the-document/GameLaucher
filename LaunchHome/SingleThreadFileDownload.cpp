#include "SingleThreadFileDownload.h"

//Biến lưu giữ các event đánh dấu trạng thái.
std::unordered_map<HINTERNET, HANDLE> hConnectEvent;
std::unordered_map<HINTERNET, HANDLE> hRequestOpenEvent;
std::unordered_map<HINTERNET, HANDLE> hRequestCompleteEvent;

HANDLE globalHConnectEvent;
HANDLE globalHRequestOpenEvent;
HANDLE globalHRequestCompleteEvent;

//Biến lưu giữ trạng thái handle.
INTERNET_ASYNC_RESULT* asyncResult;

void CALLBACK CallBack(
	__in HINTERNET hInternet,
	__in DWORD_PTR dwContext,
	__in DWORD dwInternetStatus,
	__in_bcount(dwStatusInformationLength) LPVOID lpvStatusInformation,
	__in DWORD dwStatusInformationLength
)
{
	InternetCookieHistory cookieHistory;

	/*cout << "\nOK\n";*/
	UNREFERENCED_PARAMETER(dwStatusInformationLength);
	//fprintf(stderr, "\nCallback Received for Handle %p \t", hInternet);
	if (hConnectEvent[hInternet] == NULL)
		hConnectEvent[hInternet] = CreateEvent(NULL, FALSE, FALSE, NULL);
	switch (dwInternetStatus)
	{
		case INTERNET_STATUS_HANDLE_CREATED:
		{
			SetEvent(globalHConnectEvent);
			SetEvent(hConnectEvent[hInternet]);
			break;
		}
		case INTERNET_STATUS_REQUEST_COMPLETE:
		{
			SetEvent(globalHRequestCompleteEvent);
			SetEvent(hRequestCompleteEvent[hInternet]);
			asyncResult = (INTERNET_ASYNC_RESULT*)lpvStatusInformation;
			break;
		}
	}
}

SingleThreadFileDownload::SingleThreadFileDownload(HINTERNET _internet, const wchar_t* _url, const wchar_t* _pathToFile,
	ULONGLONG _beginRange, ULONGLONG _endRange
)
{
	//Gán hết những gì cần gán ngay từ ban đầu trước.
	beginRange = _beginRange;
	endRange = _endRange;
	url = new wchar_t[250];
	pathToFile = new wchar_t[MAX_PATH];

	wcscpy(url, _url);
	wcscpy(pathToFile, _pathToFile);

	//Nếu chưa có kết nối internet thì tự tạo thôi.
	if (_internet == NULL)
	{
		DWORD internetFlag;
		InternetGetConnectedState(&internetFlag, 0);
		CHAR agentStr[64];
		sprintf_s(agentStr, "Agent%ld", timeGetTime());
		if ((internetFlag & INTERNET_CONNECTION_PROXY))
		{
			_internet = InternetOpenA(agentStr, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, INTERNET_FLAG_ASYNC);
		}
		else
		{
			_internet = InternetOpenA(agentStr, INTERNET_OPEN_TYPE_PRECONFIG_WITH_NO_AUTOPROXY, NULL, NULL, INTERNET_FLAG_ASYNC);
		}
	}
	if (!_internet)
	{
		_internet = NULL;
	}
	internet = _internet;

	globalHConnectEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	globalHRequestOpenEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	globalHRequestCompleteEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	INTERNET_STATUS_CALLBACK CallbackPointer;
	CallbackPointer = InternetSetStatusCallback(internet, (INTERNET_STATUS_CALLBACK)CallBack);

	if (CallbackPointer == INTERNET_INVALID_STATUS_CALLBACK)
	{
		fprintf(stderr, "InternetSetStatusCallback failed with INTERNET_INVALID_STATUS_CALLBACK\n");
	}

	//Lấy dung lượng cần tải về (sau khi tạo header).
	GetFileSize();

	//Tạo buffer.
	lpBuffer = new VOID*[dwSize];

	ProcessRange();
	MakeHeader();
	MakeConnection();
}

bool SingleThreadFileDownload::TerminateDownload()
{
	isExiting = true;
	if (isRunning)
		return false;

	std::ofstream outInfo;
	outInfo.open(pathToInfoFile, std::ios::out);
	//Ghi lại số byte đã tải được xuống đĩa cứng.
	if (outInfo.is_open())
	{
		outInfo << downloadedBytes << std::endl;
		if (outInfo.fail())
			std::cout << "\nFailed to write file !\n";
		outInfo.close();
		file.Flush();
		return true;
	}
}

bool SingleThreadFileDownload::UnpackDownloadedFile()
{
	CString cstring;
	CT2A ascii(cstring);
	char *url = ascii.m_psz;
	char *file_name = strrchr(url, '/') + 1;
	cstring = _T("Extracting file... ");
	cstring += file_name;

	CString unpack_path;
	// Giải nén file .zip
	SHELLEXECUTEINFO ShExecInfo = { 0 };
	ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
	ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
	ShExecInfo.hwnd = NULL;
	ShExecInfo.lpVerb = NULL;
	ShExecInfo.lpFile = _T("Unpack.exe");

	//Command extract
	WCHAR str3[MAX_PATH + 50];
	char cmd[MAX_PATH + MAX_PATH];
	strcpy(cmd, " x ../Patch/");
	strcat(cmd, file_name);
	strcat(cmd, " -o../ -aoa");
	//mbstowcs(wtext, text, strlen(text)+1);
	MultiByteToWideChar(CP_ACP, 0, cmd, -1, str3, 4096);
	ShExecInfo.lpParameters = str3;

	ShExecInfo.lpDirectory = NULL;
	ShExecInfo.nShow = SW_HIDE;
	ShExecInfo.hInstApp = NULL;
	ShellExecuteEx(&ShExecInfo);
	WaitForSingleObject(ShExecInfo.hProcess, INFINITE);
	cstring = unpack_path;
	DeleteFile(cstring);

	//Reset busy status
	return TRUE;
}

SingleThreadFileDownload::~SingleThreadFileDownload()
{
	if (header != nullptr && header != NULL)
		delete header;
	if (pathToFile != nullptr)
		delete pathToFile;
	if (pathToInfoFile != nullptr)
		delete pathToInfoFile;
	if (url != nullptr)
		delete url;
	if (lpBuffer != nullptr && lpBuffer != NULL)
		delete lpBuffer;
	try
	{
		file.Close();
	}
	catch (...) { std::cout << "\nDownloaded file invalid to close\n"; }
}

void SingleThreadFileDownload::GetFileSize()
{
	//Lấy thông tin kích thước file cần tải về.
	//Đầu tiên phải lấy thông tin file về xem nặng bao nhiêu đã.
	//Bằng cách tạo ra 1 kết nối tạm để lấy thông tin.
	HINTERNET tempConn;
	//Options cho việc mở kết nối với URL.
	DWORD connectionOptions = INTERNET_FLAG_NEED_FILE | INTERNET_FLAG_HYPERLINK | INTERNET_FLAG_RESYNCHRONIZE | INTERNET_FLAG_RELOAD;
	bool connectionResult = tempConn = InternetOpenUrlW(internet, url, NULL, NULL, connectionOptions, 1);

	DWORD error = GetLastError();
	//Chờ đến khi nào nhận được handle connection.
	if (!connectionResult && error == ERROR_IO_PENDING)
	{
		if (WaitForSingleObject(globalHRequestCompleteEvent, dwDefaultTimeout) == WAIT_TIMEOUT)
		{
			std::cout << "\nRequest timeout for getting content length !";
		}
		else
		{
			std::cout << "\nRequest completed successfully !";
			tempConn = (HINTERNET)(asyncResult->dwResult);
		}
	}

	DWORD sizeOfFileSize = 4; //Int.

	while (!HttpQueryInfo(tempConn, HTTP_QUERY_CONTENT_LENGTH | HTTP_QUERY_FLAG_NUMBER, (LPVOID)&downloadSize, &sizeOfFileSize, NULL))
	{
		DWORD errorCode = GetLastError();
		if (errorCode == ERROR_INSUFFICIENT_BUFFER)
		{
			std::cout << "\nInsufficent File Size Buffer !";
			downloadSize = 0;
			break;
		}
		else
		{
			downloadSize = 0;
			break;
		}
	}

	InternetCloseHandle(tempConn);
}

void SingleThreadFileDownload::MakeConnection()
{
	DWORD connectionOptions = INTERNET_FLAG_NEED_FILE | INTERNET_FLAG_HYPERLINK | INTERNET_FLAG_RESYNCHRONIZE | INTERNET_FLAG_RELOAD;
	bool connectionResult;
	if (header != NULL)
		connectionResult = connection = InternetOpenUrlW(internet, url, header, -1L, connectionOptions, 1);
	else
		connectionResult = connection = InternetOpenUrlW(internet, url, NULL, NULL, connectionOptions, 1);

	//Biến lưu lại lỗi.
	DWORD error = GetLastError();
	if (!connectionResult && error == ERROR_IO_PENDING)
	{
		if (WaitForSingleObject(globalHConnectEvent, dwDefaultTimeout) == WAIT_TIMEOUT)
		{
			std::cout << "\nTimeout for connection " << " in connecting event !";
		}
		if (WaitForSingleObject(globalHRequestCompleteEvent, dwDefaultTimeout) == WAIT_TIMEOUT)
		{
			std::cout << "\nTimeout for connection " << " in completing request event !";
		}
		else
		{
			connection = (HINTERNET)(asyncResult->dwResult);
			std::cout << "\nConnection " << " : " << connection << "\n\n\n";
		}
	}
	else
		connection = NULL;
}

void SingleThreadFileDownload::ProcessRange()
{
	//Nếu người dùng chỉnh beginRange == endRange tức là người dùng muốn tải về toàn bộ.
	if (beginRange == endRange)
	{
		beginRange = 0;
		endRange = ULONG_MAX;
	}

	header = new wchar_t[250];

	//Xem xem file thông tin có tồn tại hay không.
	//File thông tin sẽ có dạng tênfile.sav.

	pathToInfoFile = new wchar_t[MAX_PATH];
	wcscpy(pathToInfoFile, pathToFile);
	wcscat(pathToInfoFile, L".sav");

	std::ifstream ifInf;

	ifInf.open((pathToInfoFile), std::ios::in);
	if (ifInf.fail())
	{
		ifInf.close();
		ifInf.open((pathToInfoFile), std::ios::trunc | std::ios::in | std::ios::out);
	}

	//Đọc số byte đã tải về từ file.
	char sizeDownloaded[250];
	ifInf >> sizeDownloaded;

	//Đọc xong thì đóng kết nối luôn.
	ifInf.close();

	//Mặc định sẽ tải từ vị trí beginRange tới vị trí endRange.
	//Trong trường hợp đã tải được 1 lượng nhất định thì ta tải tiếp chứ không tải lại từ đầu.
	ULONGLONG downloadedSize = atoi(sizeDownloaded);

	bool isFilePrevDownloadedCorrupt = false;

	if (downloadedSize != 0)
	{
		//Đầu tiên lấy kích thước file đã tải về.
		std::wstring ws(pathToFile);
		ULONGLONG fileDownloadedPrevSize = GetFileSize((pathToFile));
		//Download lỗi.
		if (fileDownloadedPrevSize != downloadedSize)
			isFilePrevDownloadedCorrupt = true;

	}
	if (isFilePrevDownloadedCorrupt)
	{
		beginRange = 0;
		downloadedBytes = 0;
		_wremove(pathToFile);
		_wremove(pathToInfoFile);
	}
	else
	{
		beginRange += downloadedSize;
		downloadedBytes += downloadedSize;
	}
}

void SingleThreadFileDownload::MakeHeader()
{
	//Tạo header.
	_snwprintf(header, 250, L"Range: bytes=%lld-%lld", beginRange, endRange);
}

InternetDownloadStatus SingleThreadFileDownload::CheckStatusAndReadData()
{
	//Bật cờ để biết đang trong quá trình tải về.
	isRunning = true;

	InternetDownloadStatus returnResult;
	returnResult.totalBytes = downloadSize;
	returnResult.downloadedBytes = 0;
	returnResult.downloadedPercent = 0;

	//Kiểm tra tải xong rồi thì thôi tải chi nữa.
	if (downloadedBytes == downloadSize)
	{
		returnResult.downloadedBytes = returnResult.totalBytes;
		returnResult.downloadedPercent = 100.0;
		returnResult.statusCode = StatusCode::Success;
		return returnResult;
	}

	//Vòng lặp dùng để lấy dữ liệu về và ghi vào file.
	bool result;
	INTERNET_BUFFERS buffer;
	memset(&buffer, 0, sizeof(buffer));
	buffer.dwStructSize = sizeof(buffer);
	buffer.lpvBuffer = lpBuffer;
	buffer.dwBufferLength = dwSize - 1;
	if (!(result = InternetReadFileEx(connection, &buffer, IRF_ASYNC, 1)))
	{
		DWORD error = GetLastError();
		if (error == ERROR_IO_PENDING)
		{
			if (WaitForSingleObject(globalHRequestCompleteEvent, dwDefaultTimeout) == WAIT_TIMEOUT)
			{
				std::cout << "\nTimeout at downloading at connection : " + 0 << " ! ";
				returnResult.statusCode = StatusCode::TimedOutReadingData;
			}
			else
			{
				std::cout << "\nNOOOOOOOOOOOO\n\n\n";
			}
		}
	}
	//Tải được rồi thì phải reset đi chớ.
	//ResetEvent(hRequestCompleteEvent[connection]);

	if (!result && buffer.dwBufferLength == 0)
	{
		returnResult.downloadedBytes = returnResult.totalBytes;
		returnResult.downloadedPercent = 100.0;
		returnResult.statusCode = StatusCode::Success;
		return returnResult;
	}
	//Ghi vào file.
	try
	{
		file.Write(lpBuffer, buffer.dwBufferLength);
	}
	catch (...)
	{
		std::cout << "\nCannot write file. Maybe disk full ?\n";
	}
	downloadedBytes += buffer.dwBufferLength;
	returnResult.totalBytes = downloadSize;
	returnResult.downloadedBytes = downloadedBytes;
	returnResult.downloadedPercent = 100 * (double)returnResult.downloadedBytes / (double)returnResult.totalBytes;
	returnResult.statusCode = StatusCode::Success;

	//Tải xong rồi thì tắt cờ.
	isRunning = false;

	//Lưu lại luôn để biết là tải xong rồi đừng tải nữa.
	TerminateDownload();
	
	return returnResult;
}

bool SingleThreadFileDownload::SetupDownload()
{
	//Mở file ra để tải về và lưu.
	if (file.Open((pathToFile), CFile::modeWrite | CFile::modeCreate | CFile::modeNoTruncate) == FALSE)
	{
		std::cout << "\nCannot open file for writing !\n";
		return false;
	}
	file.SeekToEnd();
	return true;
}