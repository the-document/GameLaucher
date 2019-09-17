#pragma once
#include <afxext.h>
#include"resource.h"


class MyButton :
	public CBitmapButton
{
private:
	int m_hoverBitmap;
	int m_nomalBitmap;

protected:
	COLORREF     m_clrTrans;

public:
	MyButton();
	virtual ~MyButton();
	DECLARE_MESSAGE_MAP()

	afx_msg void OnMouseHover(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMouseLeave();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	
	virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);
	
public:
	void SetHoverBitmap(int idResource);
	void SetNomalBitmap(int idResource);
};

