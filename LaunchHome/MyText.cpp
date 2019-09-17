#include "stdafx.h"
#include "MyText.h"


MyText::MyText()
{
	
	font.CreateFontW(15,                        // nHeight
		0,                         // nWidth
		0,                         // nEscapement
		0,                         // nOrientation
		FW_NORMAL,                 // nWeight
		FALSE,                     // bItalic
		FALSE,                     // bUnderline
		0,                         // cStrikeOut
		ANSI_CHARSET,              // nCharSet
		OUT_DEFAULT_PRECIS,        // nOutPrecision
		CLIP_DEFAULT_PRECIS,       // nClipPrecision
		DEFAULT_QUALITY,           // nQuality
		DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
		_T("Arial"));                 // lpszFacename
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
	ON_WM_PAINT()
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


void MyText::OnPaint()
{
	this->SetFont(&font, false);
	CStatic::OnPaint(); // device context for painting
					   // TODO: Add your message handler code here
					   // Do not call CStatic::OnPaint() for painting messages
}
