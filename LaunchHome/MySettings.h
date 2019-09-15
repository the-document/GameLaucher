#pragma once
class MySettings
{
private:
	static MySettings* instance;
	MySettings();

public:
	static MySettings* Instance();
	~MySettings();

	LPWSTR homeUrl;
	LPWSTR registerUrl;
	LPWSTR tokenUrl;
	LPWSTR groupUrl;
	LPWSTR exitUrl;
};

