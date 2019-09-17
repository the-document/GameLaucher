#include "stdafx.h"
#include "MySettings.h"
MySettings* MySettings::instance;

MySettings::MySettings()
{
	this->homeUrl = L"https://www.facebook.com/nguyenhongphuc1998";
	this->registerUrl = L"https://www.facebook.com/nguyenhongphuc1998";
	this->tokenUrl = L"https://www.facebook.com/nguyenhongphuc1998";
	this->groupUrl = L"https://www.facebook.com/nguyenhongphuc1998";

	this->PatchInfo = new char[260];
	this->NewLauncher = new char[260];

	this->PatchInfo = "http://45.119.212.250/tlbb/patchinfo.txt";
	this->NewLauncher = "http://45.119.212.250/tlbb/newlaunch.zip";

	this->Help_URL = new char[260];
	this->LoginServer = new char[260];

	this->Help_URL = "http://45.119.212.250/Launchers/Index.html";
	this->LoginServer = "http://45.119.212.250/tlbb/loginserver.txt";
}


MySettings * MySettings::Instance()
{
	if (!instance)
		instance = new MySettings();
	return instance;
}

MySettings::~MySettings()
{
}
