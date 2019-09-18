#pragma once

enum StatusCode
{
	Success,
	TimedOutReadingData,
	CannotWriteToFile
};

struct InternetDownloadStatus
{
	unsigned long long totalBytes;
	unsigned long long downloadedBytes;
	double downloadedPercent;
	StatusCode statusCode;
};