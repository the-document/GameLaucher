#include "stdafx.h"
#include "MySettings.h"
MySettings* MySettings::instance;

MySettings::MySettings()
{
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
