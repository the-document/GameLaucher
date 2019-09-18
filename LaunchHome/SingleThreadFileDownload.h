#pragma once
#define _AFXDLL
#include <afx.h>
#include <WinInet.h>
#include <mmsystem.h>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include <shellapi.h>
#include "InternetDownloadStatus.h"

#pragma comment(lib, "Wininet.lib")
#pragma comment(lib, "winmm.lib")

class SingleThreadFileDownload
{
private:

	//Biến lưu giữ giá trị timeout mặc định.
	const DWORD dwDefaultTimeout = 20000;
	
	//Biến lưu giữ kích thước của buffer tải về.
	const DWORD dwSize = 1024;

	//Biến lưu giữ số byte đã tải về.
	ULONGLONG downloadedBytes = 0;

	//Biến lưu giữ buffer.
	VOID** lpBuffer = nullptr;

	//Biến lưu giữ file trên đĩa cứng.
	CFile file;

	//Biến lưu giữ kết nối internet.
	HINTERNET internet;

	//Biến lưu giữ URL cần tải về.
	wchar_t * url = nullptr;

	//Biến lưu giữ kết nối đến URL.
	HINTERNET connection;

	//Biến lưu giữ khoảng bắt đầu tải và khoảng kết thúc tải.
	unsigned long long beginRange = 0;
	unsigned long long endRange = 0;

	//Biến lưu giữ header tuỳ chọn để submit chung với file.
	wchar_t * header;

	//Biến lưu giữ kích thước cần tải về.
	UINT downloadSize;

	//Biến lưu giữ đường dẫn đến nơi tải về.
	wchar_t* pathToFile = nullptr;

	//Biến lưu trữ đường dẫn đến nơi chứa file info của file tải về.
	wchar_t* pathToInfoFile = nullptr;

	//Biến báo hiệu rằng chương trình đang tắt.
	bool isExiting = false;

	//Biến báo hiệu chương trình vẫn còn đang trong quá trình tải.
	bool isRunning = false;

public:
	SingleThreadFileDownload(HINTERNET _internet, const wchar_t* _url, const wchar_t* _pathToFile, ULONGLONG _beginRange = 0, ULONGLONG _endRange = 0);
	~SingleThreadFileDownload();

	bool SetupDownload();
	InternetDownloadStatus CheckStatusAndReadData();
	bool UnpackDownloadedFile();
	bool TerminateDownload();

private:
	//Lấy về số bytes cần tải và xác định khoảng bắt đầu tải.
	void GetFileSize();
	//Tạo header để tải một phần của file về.
	void MakeHeader();
	//Xử lý range.
	void ProcessRange();
	//Tạo kết nối để tải file về.
	void MakeConnection();
	//Hàm lấy kích thước 1 file bất kỳ.
	ULONGLONG GetFileSize(std::wstring filename)
	{
		struct _stat64i32 stat_buf;
		int rc = _wstat(filename.c_str(), &stat_buf);
		return rc == 0 ? stat_buf.st_size : -1;
	}
};