#include "stdafx.h"
#include "MySettings.h"
MySettings* MySettings::instance;

MySettings::MySettings()
{
	this->homeUrl = L"https://www.facebook.com/nguyenhongphuc1998";
	this->registerUrl = L"https://www.facebook.com/nguyenhongphuc1998";
	this->tokenUrl = L"https://www.facebook.com/nguyenhongphuc1998";
	this->groupUrl = L"https://www.facebook.com/nguyenhongphuc1998";
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
