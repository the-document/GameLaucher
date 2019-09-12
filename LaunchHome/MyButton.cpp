#include "stdafx.h"
#include "MyButton.h"


#define AFX_PNG_RESOURCE_TYPE  _T("PNG")


MyButton::MyButton()
{
	m_clrTrans = RGB(255, 255, 255);
}


MyButton::~MyButton()
{
}
BEGIN_MESSAGE_MAP(MyButton, CBitmapButton)
	ON_WM_MOUSEHOVER()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSELEAVE()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


void MyButton::OnMouseHover(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	
	this->LoadBitmaps(this->m_hoverBitmap, this->m_hoverBitmap, 0, this->m_hoverBitmap);
	this->RedrawWindow();
	OutputDebugString(L"\nonhover\n");
	
	CButton::OnMouseHover(nFlags, point);
}


void MyButton::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	TRACKMOUSEEVENT tme;
	tme.cbSize = sizeof(tme);
	tme.hwndTrack = m_hWnd;
	tme.dwFlags = TME_LEAVE | TME_HOVER;
	tme.dwHoverTime = 1;
	TrackMouseEvent(&tme);
	CBitmapButton::OnMouseMove(nFlags, point);
}


void MyButton::OnMouseLeave()
{
	// TODO: Add your message handler code here and/or call default
	this->LoadBitmaps(this->m_nomalBitmap, this->m_nomalBitmap, 0, this->m_nomalBitmap);
	this->RedrawWindow();
	CBitmapButton::OnMouseLeave();
}


BOOL MyButton::OnEraseBkgnd(CDC* pDC)
{ 
	// TODO: Add your message handler code here and/or call default
	//CRect clientRect;

	//GetClientRect(&clientRect);
	//pDC->FillSolidRect(clientRect, RGB(255, 255, 255));  // paint background in magenta
	//return CBitmapButton::OnEraseBkgnd(pDC);
	return false;
}
VOID DrawBitmapTrans(IN OUT CDC* pDC, IN CBitmap* pcBitmap, IN int x, IN int y, IN COLORREF crMask);
void MyButton::DrawItem(LPDRAWITEMSTRUCT lpDIS)
{
	ASSERT(lpDIS != NULL);
	// must have at least the first bitmap loaded before calling DrawItem  
	ASSERT(m_bitmap.m_hObject != NULL);     // required  

	// use the main bitmap for up, the selected bitmap for down  
	CBitmap* pBitmap = &m_bitmap;
	UINT state = lpDIS->itemState;
	if ((state & ODS_SELECTED) && m_bitmapSel.m_hObject != NULL)
		pBitmap = &m_bitmapSel;
	else if ((state & ODS_FOCUS) && m_bitmapFocus.m_hObject != NULL)
		pBitmap = &m_bitmapFocus;   // third image for focused  
	else if ((state & ODS_DISABLED) && m_bitmapDisabled.m_hObject != NULL)
		pBitmap = &m_bitmapDisabled;   // last image for disabled  
	/*if ((TRUE == m_bMouseHover) && NULL != m_cBitmapHover.GetSafeHandle())
	{
		pBitmap = &m_cBitmapHover;
	}*/

	// draw the whole button  
	CDC* pDC = CDC::FromHandle(lpDIS->hDC);

	CRect rect;
	rect.CopyRect(&lpDIS->rcItem);

	DrawBitmapTrans(pDC, pBitmap, rect.left, rect.top, m_clrTrans);

}

void MyButton::SetHoverBitmap(int idResource)
{
	this->m_hoverBitmap = idResource;
}

void MyButton::SetNomalBitmap(int idResource)
{
	this->m_nomalBitmap = idResource;
}

VOID DrawBitmapTrans(IN OUT CDC* pDC, IN CBitmap* pcBitmap, IN int x, IN int y, IN COLORREF crMask)
{
	CBitmap     *pOldBitmapImage = NULL;
	CBitmap     *pOldBitmapTrans = NULL;
	COLORREF    clrOldBack = 0;
	COLORREF    clrOldText = 0;
	BITMAP      bm = { 0, };
	CDC         dcImage;
	CDC         dcTrans;
	CBitmap     bitmapTrans;

	if ((NULL == pcBitmap) || (NULL == pDC))
	{
		return;
	}

	pcBitmap->GetBitmap(&bm);
	dcImage.CreateCompatibleDC(pDC);
	pOldBitmapImage = dcImage.SelectObject(pcBitmap);
	clrOldBack = pDC->SetBkColor(RGB(255, 255, 255));
	clrOldText = pDC->SetTextColor(RGB(0, 0, 0));
	dcTrans.CreateCompatibleDC(pDC);
	bitmapTrans.CreateBitmap(bm.bmWidth, bm.bmHeight, 1, 1, NULL);
	pOldBitmapTrans = dcTrans.SelectObject(&bitmapTrans);
	dcImage.SetBkColor(crMask);
	dcTrans.BitBlt(0, 0, bm.bmWidth, bm.bmHeight, &dcImage, 0, 0, SRCCOPY);
	pDC->BitBlt(x, y, bm.bmWidth, bm.bmHeight, &dcImage, 0, 0, SRCINVERT);
	pDC->BitBlt(x, y, bm.bmWidth, bm.bmHeight, &dcTrans, 0, 0, SRCAND);
	pDC->BitBlt(x, y, bm.bmWidth, bm.bmHeight, &dcImage, 0, 0, SRCINVERT);
	dcTrans.SelectObject(pOldBitmapTrans);
	pDC->SetBkColor(clrOldBack);
	pDC->SetTextColor(clrOldText);
	bitmapTrans.DeleteObject();
	dcTrans.DeleteDC();
	dcImage.SelectObject(pOldBitmapImage);
	dcImage.DeleteDC();
}