#include "stdafx.h"
#include "MyText.h"


MyText::MyText()
{
	
}


MyText::~MyText()
{
}
void MyText::SetTextColor(COLORREF color)
{
	this->m_TextColor = color;
}

BEGIN_MESSAGE_MAP(MyText, CStatic)
	ON_WM_CTLCOLOR_REFLECT()
END_MESSAGE_MAP()



HBRUSH MyText::CtlColor(CDC* pDC, UINT /*nCtlColor*/)
{
	/*CFont m_Font;
	m_Font.CreatePointFont(2, L"Verdana");
	this->SetFont(&m_Font);*/

	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor(m_TextColor);
	return (HBRUSH)GetStockObject(NULL_BRUSH);
}
