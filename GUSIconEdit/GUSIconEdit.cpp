/********************************************************************
	Created        : 2011/05/29
	Filename       : GUSIconEdit.cpp
    Original File  : IconEdit.cpp
	Original Author: Gautam Jain
    Modified By    : Yaukey(yaukeywang@gmail.com)

    Licence: You can use and modify this code for any propose,
             but to read the original disclaimer that this code has to
             include:

    DISCLAIMER:
             The code in this project is Copyright (C) 2006 by Gautam Jain. 
             You have the right to use and distribute the code in any way 
             you see fit as long as this paragraph is included with the 
             distribution. No warranties or claims are made as to the 
             validity of the information and code contained herein, 
             so use it at your own risk.
*********************************************************************/

#include "stdafx.h"
#include "GUSIconEdit.h"

//////////////////////////////////////////////////////////////////////////
// Constant definition

const int N_GUS_ICON_EDIT_MIN_WIDTH  = 20;          // The minimum width of editable area of edit.
const int N_GUS_ICON_EDIT_MIN_HEIGHT = 14;          // The minimum height of editable area of edit.

//////////////////////////////////////////////////////////////////////////
// CIcon class.

IMPLEMENT_DYNAMIC(CGUSIconEdit, CEdit)

CGUSIconEdit::CGUSIconEdit()
{
	CSize sizeTmp(0, 0);
	CRect rcTmp(0, 0, 0, 0);

	m_sizeThemePic = sizeTmp;

	m_rcLeftIconEmpty   = rcTmp;
	m_rcLeftIconFill    = rcTmp;
	m_rcMiddleIconEmpty = rcTmp;
	m_rcMiddleIconFill  = rcTmp;
	m_rcRightIconEmpty  = rcTmp;
	m_rcRightIconFill   = rcTmp;

	m_rcLeftBtnArea        = rcTmp;
	m_rcRightBtnArea       = rcTmp;
    m_rcRightBtnOriginArea = rcTmp;

	m_rcEditArea = rcTmp;
	m_rcEditSize = rcTmp;

	m_clrTxt       = RGB(0, 0, 0);
	m_clrEmptyTips = RGB(0, 0, 0);
	m_clrBk        = RGB(255, 255, 255);

	m_cClrBrush.CreateSolidBrush(RGB(255, 255, 255));

	m_bEmpty                = TRUE;
	m_bIsRightBtnAlwaysShow = FALSE;

	m_bSearchCaseSense = FALSE;
	m_bSearchAnywhere  = TRUE;
}

CGUSIconEdit::~CGUSIconEdit()
{
	m_bmpThemePic.DeleteObject();
	m_cClrBrush.DeleteObject();
}

BOOL CGUSIconEdit::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	DWORD dwRealStyle = dwStyle;

	dwRealStyle &= (~WS_BORDER);
	dwRealStyle &= (~WS_POPUP);
	dwRealStyle |= ES_MULTILINE;

	if (!CEdit::Create(dwRealStyle, rect, pParentWnd, nID))
	{
		return FALSE;
	}

	SetFont(pParentWnd->GetFont());
	return TRUE;
}

BOOL CGUSIconEdit::InitializeResource(
	UINT   iEditTheme, 
	CRect& rcLIconEmpty, 
	CRect& rcLIconFill,
	CRect& rcMIconEmpty,
	CRect& rcMIconFill,
	CRect& rcRIconEmpty,
	CRect& rcRIconFill,
	CRect& rcLBtn,
	CRect& rcRBtn
	)
{
	if (UINT_MAX == iEditTheme)
	{
		return FALSE;
	}

	if (NULL == GetParent())
	{
		return FALSE;
	}

	// Create the search list.
	if (!m_cSearchListWnd.CreateAndInit(this))
	{
		return FALSE;
	}

	// Load bitmap info.

	m_bmpThemePic.DeleteObject();
	if (!m_bmpThemePic.LoadBitmap(iEditTheme))
	{
		return FALSE;
	}

	BITMAP bmpInfo;
	m_bmpThemePic.GetBitmap(&bmpInfo);
	m_sizeThemePic.SetSize(bmpInfo.bmWidth, bmpInfo.bmHeight);

	// Set all of the proper rect.
	m_rcLeftIconEmpty      = rcLIconEmpty;
	m_rcLeftIconFill       = rcLIconFill;
	m_rcMiddleIconEmpty    = rcMIconEmpty;
	m_rcMiddleIconFill     = rcMIconFill;
	m_rcRightIconEmpty     = rcRIconEmpty;
	m_rcRightIconFill      = rcRIconFill;
	m_rcLeftBtnArea        = rcLBtn;
    m_rcRightBtnArea       = rcRBtn;
    m_rcRightBtnOriginArea = rcRBtn;

	// Resize the window.
	ResizeWindow();

	return TRUE;
}

void CGUSIconEdit::ShowRightButtonAlways(BOOL bAlways)
{
	m_bIsRightBtnAlwaysShow = bAlways;
}

COLORREF CGUSIconEdit::SetTextColor(COLORREF clrTxt)
{
	COLORREF clrTmp = m_clrTxt;
	m_clrTxt = clrTxt;

	UpdateContent();

	return clrTmp;
}

COLORREF CGUSIconEdit::SetBkColor(COLORREF clrBk)
{
	COLORREF clrTmp = m_clrBk;
	m_clrBk = clrBk;

	UpdateContent();

	return clrTmp;
}

void CGUSIconEdit::SetEmptyTips(LPCTSTR strTips, COLORREF clrTips)
{
	m_strEmptyTips = strTips;
	m_clrEmptyTips = clrTips;

	SetFocus();
	UpdateContent();
}

void CGUSIconEdit::SetEditText(LPCTSTR strText)
{
	SetWindowText(strText);
	UpdateContent(FALSE);
}

void CGUSIconEdit::GetEditText(CString& strText)
{
	if (m_bEmpty)
	{
		strText.Empty();
	}
	else
	{
		GetWindowText(strText);
	}
}

int CGUSIconEdit::GetEditTextLength()
{
	if (m_bEmpty)
	{
		return 0;
	}
	else
	{
		return GetWindowTextLength();
	}
}

void CGUSIconEdit::ClearEditText()
{
	SetWindowText(_T(""));
	UpdateContent(FALSE);

	m_cSearchListWnd.ShowWindow(SW_HIDE);
}

void CGUSIconEdit::AddSearchString(LPCTSTR lpszString, DWORD dwData)
{
	m_cSearchListWnd.AddSearchString(lpszString, dwData);
}

void CGUSIconEdit::RemoveAt(int nItem)
{
	m_cSearchListWnd.RemoveAt(nItem);
}

void CGUSIconEdit::RemoveAll()
{
	m_cSearchListWnd.RemoveAll();
}

int CGUSIconEdit::GetSourcesCount() const
{
    return m_cSearchListWnd.GetSourcesCount();
}

BOOL CGUSIconEdit::GetSourceText(int nItem, CString& strText)
{
    return m_cSearchListWnd.GetSourceText(nItem, strText);
}

DWORD CGUSIconEdit::GetSourceData(int nItem)
{
    return m_cSearchListWnd.GetSourceData(nItem);
}

int CGUSIconEdit::GetResultsCount() const
{
    return m_cSearchListWnd.GetResultsCount();
}

LPCTSTR CGUSIconEdit::GetSelectedString() const
{
	return m_cSearchListWnd.GetSelectedString();
}

DWORD CGUSIconEdit::GetSelectedData() const
{
	return m_cSearchListWnd.GetSelectedData();
}

BOOL CGUSIconEdit::SetSearchCaseSense(BOOL bCaseSense)
{
    BOOL bOldCaseSense = m_bSearchCaseSense;
	m_bSearchCaseSense = bCaseSense;

    return bOldCaseSense;
}

BOOL CGUSIconEdit::SetSearchAnywhere(BOOL bAnywhere)
{
    BOOL bOldAnywhere = m_bSearchAnywhere;
	m_bSearchAnywhere = bAnywhere;

    return bOldAnywhere;
}

BYTE CGUSIconEdit::SetMaxDisplayItem(BYTE yItem)
{
    return m_cSearchListWnd.SetMaxDisplayItem(yItem);
}

COLORREF CGUSIconEdit::SetNormalItemsBKColor(COLORREF clrNormalItemsBK)
{
    return m_cSearchListWnd.SetNormalItemsBKColor(clrNormalItemsBK);
}

COLORREF CGUSIconEdit::SetEvenItemsBKColor(COLORREF clrEvenItemsBK)
{
    return m_cSearchListWnd.SetEvenItemsBKColor(clrEvenItemsBK);
}
 
COLORREF CGUSIconEdit::SetNonSelItemsKeywordsBKColor(COLORREF clrNonSelItemsKeywordsBK)
{
    return m_cSearchListWnd.SetNonSelItemsKeywordsBKColor(clrNonSelItemsKeywordsBK);
}

COLORREF CGUSIconEdit::SetNormalItemsTextColor(COLORREF clrNormalItemsText)
{
    return m_cSearchListWnd.SetNormalItemsTextColor(clrNormalItemsText);
}

COLORREF CGUSIconEdit::SetSelItemBKColor(COLORREF clrSelItemBK, BOOL bAutoSetSelKeywordsColor)
{
    return m_cSearchListWnd.SetSelItemBKColor(clrSelItemBK, bAutoSetSelKeywordsColor);
}

COLORREF CGUSIconEdit::SetSelItemKeywordsBKColor(COLORREF clrSelItemKeywordsBK)
{
    return m_cSearchListWnd.SetSelItemKeywordsBKColor(clrSelItemKeywordsBK);
}

COLORREF CGUSIconEdit::SetSelItemTextColor(COLORREF clrSelItemText)
{
    return m_cSearchListWnd.SetSelItemTextColor(clrSelItemText);
}

BOOL CGUSIconEdit::SetRememberWindowNewSize(BOOL bRemember)
{
	return m_cSearchListWnd.SetRememberWindowNewSize(bRemember);
}

BOOL CGUSIconEdit::SetSortDisplayedSearchResults(BOOL bSort, BOOL bAscend)
{
    return m_cSearchListWnd.SetSortDisplayedSearchResults(bSort, bAscend);
}

BOOL CGUSIconEdit::DisabePopup(BOOL bDisable)
{
    return m_cSearchListWnd.DisabePopup(bDisable);
}

void CGUSIconEdit::ResizeEditArea()
{
    // Reset the actual editable area size of edit first.
    m_rcEditArea.SetRect(0, 0, 0, 0);
    m_rcEditSize.SetRect(0, 0, 0, 0);

    // Check the left, middle and right button area.
    if (
        m_rcLeftIconEmpty.EqualRect(&m_rcEditArea)      ||
        m_rcLeftIconFill.EqualRect(&m_rcEditArea)       ||
        m_rcMiddleIconEmpty.EqualRect(&m_rcEditArea)    ||
        m_rcMiddleIconFill.EqualRect(&m_rcEditArea)     ||
        m_rcRightIconEmpty.EqualRect(&m_rcEditArea)     ||
        m_rcRightIconFill.EqualRect(&m_rcEditArea)      ||
        m_rcLeftBtnArea.EqualRect(&m_rcEditArea)        ||
        m_rcRightBtnArea.EqualRect(&m_rcEditArea)       ||
        m_rcRightBtnOriginArea.EqualRect(&m_rcEditArea)
        )
    {
        return;
    }

    // Get window size.
    CRect rcWnd;
    GetClientRect(&rcWnd);

    // Get the proper width of this edit.

    int nEditWidth = rcWnd.Width();
    int nIconWidth = max(m_rcLeftIconEmpty.Width(), m_rcLeftIconFill.Width()) + max(m_rcRightIconEmpty.Width(), m_rcRightIconFill.Width()) + N_GUS_ICON_EDIT_MIN_WIDTH;
    
    if (nEditWidth <= nIconWidth)
    {
        nEditWidth = nIconWidth;
    }

    // Get the proper height of this edit.

    int nEditHeight = max(m_rcLeftIconEmpty.Height(), m_rcLeftIconFill.Height());
    nEditHeight     = max(nEditHeight, m_rcMiddleIconEmpty.Height());
    nEditHeight     = max(nEditHeight, m_rcMiddleIconFill.Height());
    nEditHeight     = max(nEditHeight, m_rcRightIconEmpty.Height());
    nEditHeight     = max(nEditHeight, m_rcRightIconFill.Height());

    if (nEditHeight <= N_GUS_ICON_EDIT_MIN_HEIGHT)
    {
        nEditHeight = N_GUS_ICON_EDIT_MIN_HEIGHT + 16;
    }

    // Resize the right button area.
    m_rcRightBtnArea.left   = nEditWidth - max(m_rcRightIconEmpty.Width(), m_rcRightIconFill.Width()) + m_rcRightBtnOriginArea.left;
    m_rcRightBtnArea.right  = m_rcRightBtnArea.left + m_rcRightBtnOriginArea.Width();
    m_rcRightBtnArea.top    = m_rcRightBtnOriginArea.top;
    m_rcRightBtnArea.bottom = m_rcRightBtnOriginArea.bottom;

    // Set the final edit area.
    m_rcEditArea.left   = max(m_rcLeftIconEmpty.Width(), m_rcLeftIconFill.Width()) + 2;
    m_rcEditArea.right  = nEditWidth - max(m_rcRightIconEmpty.Width(), m_rcRightIconFill.Width()) - 2;
    m_rcEditArea.top    = ((0 == nEditHeight % 2) ? nEditHeight : (nEditHeight + 1)) / 2 - N_GUS_ICON_EDIT_MIN_HEIGHT / 2;
    m_rcEditArea.bottom = nEditHeight - (nEditHeight / 2 - N_GUS_ICON_EDIT_MIN_HEIGHT / 2);

    // Validate the final edit area.
    m_rcEditArea.left = (m_rcEditArea.left > m_rcEditArea.right)  ? m_rcEditArea.right  : m_rcEditArea.left;
    m_rcEditArea.top  = (m_rcEditArea.top  > m_rcEditArea.bottom) ? m_rcEditArea.bottom : m_rcEditArea.top;

    // Set new edit size.
    m_rcEditSize.SetRect(0, 0, nEditWidth, nEditHeight);
}

void CGUSIconEdit::ResizeWindow()
{
	if (!::IsWindow(m_hWnd))
	{
		return;
	}

    // Recalculate the actual size of edit.
    ResizeEditArea();

    // Processing only if edit area is set.
	if (m_rcEditArea == CRect(0, 0, 0, 0)) 
	{
		return;
	}

	SetWindowPos(&wndTop, 0, 0, m_rcEditSize.Width(), m_rcEditSize.Height(), SWP_NOMOVE | SWP_NOZORDER);
	SetRect(&m_rcEditArea);

	UpdateContent();
}

void CGUSIconEdit::UpdateContent(BOOL bEraseRect)
{
	CString strEdit;
	GetWindowText(strEdit);
	if (m_bEmpty && (strEdit == m_strEmptyTips))
	{
		m_bEmpty = TRUE;
	}
	else
	{
		m_bEmpty = strEdit.IsEmpty() ? TRUE : FALSE;
	}

	if (bEraseRect)
	{
		InvalidateRect(NULL);
	}
}

void CGUSIconEdit::PreSubclassWindow()
{
	// TODO: 在此添加专用代码和/或调用基类
	ASSERT(GetStyle() & ES_MULTILINE);
	ResizeWindow();

	CEdit::PreSubclassWindow();
}

BOOL CGUSIconEdit::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类

	return CEdit::PreTranslateMessage(pMsg);
}

BEGIN_MESSAGE_MAP(CGUSIconEdit, CEdit)
	ON_WM_CREATE()
	ON_WM_CTLCOLOR_REFLECT()
	ON_CONTROL_REFLECT(EN_CHANGE, &CGUSIconEdit::OnEnChange)
	ON_WM_ENABLE()
	ON_WM_ERASEBKGND()
	ON_WM_KEYDOWN()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDBLCLK()
	ON_WM_RBUTTONDOWN()
	ON_WM_RBUTTONUP()
	ON_WM_MOUSEWHEEL()
	ON_WM_SETCURSOR()
	ON_WM_KILLFOCUS()
	ON_WM_SETFOCUS()
	ON_MESSAGE(WM_SETFONT, &CGUSIconEdit::OnSetFont)
	ON_WM_SIZE()
	ON_MESSAGE(WM_USER_ACLISTWND_MESSAGE_CLICK_LIST, &CGUSIconEdit::OnClickSearchList)
END_MESSAGE_MAP()

int CGUSIconEdit::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CEdit::OnCreate(lpCreateStruct) == -1)
	{
		return -1;
	}

	// TODO:  在此添加您专用的创建代码
	ResizeWindow();

	return 0;
}

HBRUSH CGUSIconEdit::CtlColor(CDC* pDC, UINT nCtlColor)
{
	// TODO:  在此更改 DC 的任何属性
	pDC->SetBkColor(m_clrBk);
	if (m_bEmpty)
	{
		pDC->SetTextColor(m_clrEmptyTips);
	}
	else
	{
		pDC->SetTextColor(m_clrTxt);
	}
	

	return (HBRUSH)m_cClrBrush.GetSafeHandle();

	// TODO:  如果不应调用父级的处理程序，则返回非空画笔
	return NULL;
}

void CGUSIconEdit::OnEnable(BOOL bEnable)
{
	CEdit::OnEnable(bEnable);

	// TODO: 在此处添加消息处理程序代码
	if (bEnable)
	{
		if (m_bEmpty)
		{
			SetWindowText(m_strEmptyTips);
		}
	}
	else
	{
		if (m_bEmpty && (GetWindowTextLength() > 0))
		{
			SetWindowText(_T(""));
		}
	}
}

void CGUSIconEdit::OnEnChange()
{
	// TODO:  如果该控件是 RICHEDIT 控件，则它将不会
	// 发送该通知，除非重写 CEdit::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	UpdateContent();

	// Show the results list or not.
	if (m_bEmpty)
	{
		m_cSearchListWnd.ShowWindow(SW_HIDE);
	}
	else
	{
		CString strText;
		GetWindowText(strText);
		m_cSearchListWnd.FindString(strText, m_bSearchCaseSense, m_bSearchAnywhere);
	}

	// Send message to parent.
	CWnd* pParent = GetParent();
	if (NULL != pParent)
	{
		pParent->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), EN_CHANGE), (LPARAM)(pParent->m_hWnd));
	}
}

BOOL CGUSIconEdit::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	CDC      dcMemory;
	CBitmap* pOldBitmap = NULL;

	// Create an in-memory DC compatible with the
	// display DC we're using to paint.
	dcMemory.CreateCompatibleDC(pDC);
	pOldBitmap = dcMemory.SelectObject(&m_bmpThemePic);

	// Copy the bits from the in-memory DC into the on-
	// screen DC to actually do the painting.
	if (!m_bEmpty || m_bIsRightBtnAlwaysShow)
	{
		pDC->BitBlt(m_rcEditSize.left, m_rcEditSize.top, m_rcLeftIconFill.Width(), m_rcLeftIconFill.Height(), &dcMemory, m_rcLeftIconFill.left, m_rcLeftIconFill.top, SRCCOPY);
		pDC->StretchBlt(m_rcEditSize.left + m_rcLeftIconFill.Width(), m_rcEditSize.top, m_rcEditSize.Width() - m_rcLeftIconFill.Width() - m_rcRightIconFill.Width(), m_rcEditSize.Height(), &dcMemory, m_rcMiddleIconFill.left, m_rcMiddleIconFill.top, m_rcMiddleIconFill.Width(), m_rcMiddleIconFill.Height(), SRCCOPY);
		pDC->BitBlt(m_rcEditSize.right - m_rcRightIconFill.Width(), m_rcEditSize.top, m_rcRightIconFill.Width(), m_rcRightIconFill.Height(), &dcMemory, m_rcRightIconFill.left, m_rcRightIconFill.top, SRCCOPY);
	}
	else
	{
		pDC->BitBlt(m_rcEditSize.left, m_rcEditSize.top, m_rcLeftIconEmpty.Width(), m_rcLeftIconEmpty.Height(), &dcMemory, m_rcLeftIconEmpty.left, m_rcLeftIconEmpty.top, SRCCOPY);
		pDC->StretchBlt(m_rcEditSize.left + m_rcLeftIconEmpty.Width(), m_rcEditSize.top, m_rcEditSize.Width() - m_rcLeftIconEmpty.Width() - m_rcRightIconEmpty.Width(), m_rcEditSize.Height(), &dcMemory, m_rcMiddleIconEmpty.left, m_rcMiddleIconEmpty.top, m_rcMiddleIconEmpty.Width(), m_rcMiddleIconEmpty.Height(), SRCCOPY);
		pDC->BitBlt(m_rcEditSize.right - m_rcRightIconEmpty.Width(), m_rcEditSize.top, m_rcRightIconEmpty.Width(), m_rcRightIconEmpty.Height(), &dcMemory, m_rcRightIconEmpty.left, m_rcRightIconEmpty.top, SRCCOPY);
	}

	// Recovery the old dc object.
	dcMemory.SelectObject(pOldBitmap);

	return TRUE;
	//return CEdit::OnEraseBkgnd(pDC);
}

void CGUSIconEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if ((VK_ESCAPE == nChar) || (VK_RETURN == nChar))
	{
		// Select an item for the edit.
		if (m_cSearchListWnd.IsWindowVisible())
		{
			// Set the text to current edit.
			SetEditText(m_cSearchListWnd.GetSelectedString());
			SetSel(GetWindowTextLength(), -1);

			// Set message to parent window.
			CWnd* pParent = GetParent();
			if (NULL != pParent)
			{
				pParent->SendMessage(WM_USER_GUS_ICONEDIT_CLICK_SEARCH_LIST, (WPARAM)GetDlgCtrlID(), 0);
			}

			// Hide the search list.
			m_cSearchListWnd.ShowWindow(SW_HIDE);

			return;
		}
	}
	else if ((VK_DOWN  == nChar) || (VK_UP   == nChar) || 
			 (VK_PRIOR == nChar) || (VK_NEXT == nChar) || 
			 (VK_HOME  == nChar) || (VK_END  == nChar))
	{
		// Move the select item.
		if (m_cSearchListWnd.IsWindowVisible())
		{
			m_cSearchListWnd.GetNextString(nChar);
			m_cSearchListWnd.SelectItem(-1);

			return;
		}
	}

	CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CGUSIconEdit::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (!m_rcEditArea.PtInRect(point))
	{
		return;
	}

	CEdit::OnLButtonDblClk(nFlags, point);
}

void CGUSIconEdit::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	// If the icon button is clicked then send message to the
	// parent.

	if (m_rcLeftBtnArea.PtInRect(point))	// If left icon is clicked.
	{
		// Hide the search list.
		m_cSearchListWnd.ShowWindow(SW_HIDE);

		// Send message to parent.
		CWnd* pParent = GetParent();
		if (NULL != pParent)
		{
			pParent->SendMessage(WM_USER_GUS_ICONEDIT_LEFT_ICON_CLICKED, (WPARAM)GetDlgCtrlID(), 0);

			SetFocus();
			UpdateContent();
		}

		return;
	}
	else if (m_rcRightBtnArea.PtInRect(point))	// If right icon is clicked.
	{
		// Hide the search list.
		m_cSearchListWnd.ShowWindow(SW_HIDE);

		// Send message to parent.
		// It is assumed that when the text is not typed in the
		// edit control, the button will not be visible,
		// but you can override this by setting 
		// the m_bIsRightBtnAlwaysShow to TRUE.
		if (!m_bEmpty || m_bIsRightBtnAlwaysShow)
		{
			CWnd* pParent = GetParent();
			if (NULL != pParent)
			{
				pParent->SendMessage(WM_USER_GUS_ICONEDIT_RIGHT_ICON_CLICKED, (WPARAM)GetDlgCtrlID(), 0);

				SetFocus();
				UpdateContent();
			}
		}

		return;
	}

	CEdit::OnLButtonDown(nFlags, point);
}

void CGUSIconEdit::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CEdit::OnLButtonUp(nFlags, point);
}

void CGUSIconEdit::OnRButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CEdit::OnRButtonDblClk(nFlags, point);
}

void CGUSIconEdit::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CEdit::OnRButtonDown(nFlags, point);
}

void CGUSIconEdit::OnRButtonUp(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (!m_rcEditArea.PtInRect(point))
	{
		return;
	}

	CEdit::OnRButtonUp(nFlags, point);
}

BOOL CGUSIconEdit::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if (m_cSearchListWnd.IsWindowVisible())
	{
		return m_cSearchListWnd.DoMouseWheel(nFlags, zDelta, pt);
	}
	else
	{
		return CEdit::OnMouseWheel(nFlags, zDelta, pt);
	}
}

BOOL CGUSIconEdit::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	// By default, when the mouse moves over the edit control
	// the system shows the I-beam cursor. However we want to
	// show the arrow cursor when it is over the Non-Edit area
	// where the button and icon is displayed
	// here is the code to do this.

	CPoint ptCursor;
	GetCursorPos(&ptCursor);
	ScreenToClient(&ptCursor);

	// If mouse is not in the edit area then
	// show arrow cursor.
	if (!m_rcEditArea.PtInRect(ptCursor))
	{
		SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
		return TRUE;
	}

	return CEdit::OnSetCursor(pWnd, nHitTest, message);
}

void CGUSIconEdit::OnKillFocus(CWnd* pNewWnd)
{
	CEdit::OnKillFocus(pNewWnd);

	// TODO: 在此处添加消息处理程序代码
	if (m_bEmpty)
	{
		SetWindowText(m_strEmptyTips);
	}

	m_cSearchListWnd.ShowWindow(SW_HIDE);
}

void CGUSIconEdit::OnSetFocus(CWnd* pOldWnd)
{
	CEdit::OnSetFocus(pOldWnd);

	// TODO: 在此处添加消息处理程序代码
	if (m_bEmpty && (GetWindowTextLength() > 0))
	{
		SetWindowText(_T(""));
	}
}

LRESULT CGUSIconEdit::OnSetFont(WPARAM wParam, LPARAM lParam)
{
	DefWindowProc(WM_SETFONT, wParam, lParam);
	ResizeWindow();

	return S_OK;
}

void CGUSIconEdit::OnSize(UINT nType, int cx, int cy)
{
	CEdit::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	ResizeWindow();
}

LRESULT CGUSIconEdit::OnClickSearchList(WPARAM wParam, LPARAM lParam)
{
	// Set the text to current edit.
	SetEditText(m_cSearchListWnd.GetSelectedString());
	SetSel(GetWindowTextLength(), -1);

	// Set message to parent window.
	CWnd* pParent = GetParent();
	if (NULL != pParent)
	{
		pParent->SendMessage(WM_USER_GUS_ICONEDIT_CLICK_SEARCH_LIST, (WPARAM)GetDlgCtrlID(), 0);
	}

	return S_OK;
}
