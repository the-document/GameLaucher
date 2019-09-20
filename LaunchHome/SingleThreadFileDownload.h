#pragma once
#define _AFXDLL
#include <afx.h>
#include <WinInet.h>
#include <mmsystem.h>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <shellapi.h>
#include "InternetDownloadStatus.h"

#pragma comment(lib, "Wininet.lib")
#pragma comment(lib, "winmm.lib")

class SingleThreadFileDownload
{
private:

	//Biến lưu giữ giá trị timeout mặc định.
	const DWORD dwDefaultTimeout = 300000;
	
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

	//Biến báo hiệu URL không hợp lệ. Chỉ được set = true, không được set = false.
	bool invalidURL = false;

	//Biến báo hiệu đã bị lỗi ở chỗ nào đó trong quá trình connect và không thể tiếp tục. Chỉ được set = true, không được set = false.
	bool timedOutSomewhere = false;

	//Biến lưu giữ kết nối đến URL.
	HINTERNET connection;

	//Biến lưu giữ khoảng bắt đầu tải và khoảng kết thúc tải.
	unsigned long long beginRange = 0;
	unsigned long long endRange = 0;

	//Biến lưu giữ header tuỳ chọn để submit chung với file.
	wchar_t * header = nullptr;

	//Biến lưu giữ ngày cuối cùng mà file được thay đổi (trong lần tải này).
	wchar_t lastModified[250];

	//Biến lưu giữ kích thước cần tải về.
	ULONGLONG downloadSize;

	//Biến lưu giữ đường dẫn đến nơi tải về.
	wchar_t* pathToFile = nullptr;

	//Biến lưu trữ đường dẫn đến nơi chứa file info của file tải về.
	wchar_t* pathToInfoFile = nullptr;

	//Biến báo hiệu rằng chương trình đang tắt.
	bool isExiting = false;

	//Biến báo hiệu chương trình vẫn còn đang trong quá trình tải.
	bool isRunning = false;

	//Biến báo hiệu chương trình cho phép lấy một đoạn của file về.
	bool isResumable = false;

public:
	SingleThreadFileDownload(HINTERNET _internet, const wchar_t* _url, const wchar_t* _pathToFile, ULONGLONG _beginRange = 0, ULONGLONG _endRange = 0);
	~SingleThreadFileDownload();

	bool SetupDownload();
	InternetDownloadStatus CheckStatusAndReadData();
	bool UnpackDownloadedFile(wchar_t* pathToZippedFile, wchar_t* pathToUnzipFolder);
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