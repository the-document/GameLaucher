#pragma once
#include"SingleThreadFileDownload.h"
#include "stdafx.h"

class MyParam
{
public:
	SingleThreadFileDownload *thread;
	CProgressCtrl* progessDowload;
};