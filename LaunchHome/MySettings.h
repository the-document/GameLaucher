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

	wchar_t* PatchInfo;
	wchar_t* NewLauncher;
	char* LoginServer;
	char* Help_URL;

	char *FVersion = "..\\(version)"; // File (version)
	//wchar_t* FPatchInfo = L"..\\Patch\\patchinfo.txt"; // File patchinfo.txt
	char* FPatchInfo = "..\\Patch\\patchinfo.txt";
	wchar_t* FPatchLaucher = L"..\\Patch\\newlaunch.zip";
	char* FLaunchVersion = "./(LaunchVersion)"; // File (LaunchVersion)

	wchar_t* URL1;

	void SetUrl1(wchar_t*);
};

