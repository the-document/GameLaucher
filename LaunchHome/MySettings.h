#pragma once
class MySettings
{
private:
	static MySettings* instance;
	MySettings();

public:
	static MySettings* Instance();
	~MySettings();

	char* backgroundUrl;
};

