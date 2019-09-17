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

	char* PatchInfo;
	char* NewLauncher;
	char* LoginServer;
	char* Help_URL;

	char *FVersion = "..\\(version)"; // File (version)
	char *FPatchInfo = "..\\Patch\\patchinfo.txt"; // File patchinfo.txt
};

