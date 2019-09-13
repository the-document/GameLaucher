#pragma once
#include <afxwin.h>
class MyText :
	public CStatic

{
	//DECLARE_DYNAMIC(MyText)
private:
	COLORREF m_TextColor = RGB(255, 255, 255);
	
public:
	MyText();
	virtual ~MyText();

	void SetTextColor(COLORREF);

	DECLARE_MESSAGE_MAP()
	afx_msg HBRUSH CtlColor(CDC* /*pDC*/, UINT /*nCtlColor*/);
protected:
	
};

