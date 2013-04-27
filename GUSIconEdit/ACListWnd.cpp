/*********************************************************************
*
* CACListWnd.cpp 1.20
* Copyright (c) 2003-11 by Andreas Kapust
* All rights reserved.
* info@applications.de
*
* Modified by: Yaukey(yaukeywang@gmail.com).
* 
* Licence: The Code Project Open License (CPOL)
*          http://www.codeproject.com/info/cpol10.aspx
*
*********************************************************************/

#include "stdafx.h"
#include "ACListWnd.h"
#include <vector>
#include <list>
#include <algorithm>
#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////////
// The extended style.
#ifndef CS_DROPSHADOW
#define CS_DROPSHADOW	0x00020000
#endif

//////////////////////////////////////////////////////////////////////////
// Some internal data types.

// Store the byte array.
typedef std::vector<BYTE> CACListByteVector;

// Store string and data associated.
class CACListSearchNode : public std::pair<CString, DWORD>
{
public:
    CACListSearchNode()
    {
        second = UINT_MAX;
    }

    CACListSearchNode(LPCTSTR pString, DWORD dwData)
    {
        SetNode(pString, dwData);
    }

    void SetNode(LPCTSTR pString, DWORD dwData)
    {
        first  = pString;
        second = dwData;
    }
};

// Store string, data and keywords index associated for display list of search results.
class CACListDisplayNode : public std::pair<LPCTSTR, DWORD>
{
public:
    CACListDisplayNode() 
    {
        Clear();
    }

    void Clear()
    {
        first  = NULL;
        second = UINT_MAX;
        third.clear();
    }

    void SetBaseNode(const CACListSearchNode& cBase)
    {
        first  = cBase.first;
        second = cBase.second;
    }

public:
    CACListByteVector third;
};

// Store all the CACListSearchNode.
class CACListSearchList
{
public:
    std::vector<CACListSearchNode>& operator*()
    {
        return m_aArray;
    }

    std::vector<CACListSearchNode>* operator->()
    {
        return (&**this);
    }

    CACListSearchNode& operator[](int nIndex)
    {
        return m_aArray[nIndex];
    }

public:
    std::vector<CACListSearchNode> m_aArray;
};

// Store all the CACListDisplayNode.
class CACListDisplayList
{
public:
    std::vector<CACListDisplayNode*>& operator*()
    {
        return m_aReflectArray;
    }

    std::vector<CACListDisplayNode*>* operator->()
    {
        return (&**this);
    }

    CACListDisplayNode* operator[](int nIndex)
    {
        return m_aReflectArray[nIndex];
    }

    std::list<CACListDisplayNode>& GetRawData()
    {
        return m_aRealArray;
    }

    void Clear()
    {
        m_aRealArray.clear();
        m_aReflectArray.clear();
    }

public:
    std::list<CACListDisplayNode>    m_aRealArray;
    std::vector<CACListDisplayNode*> m_aReflectArray;
};

//////////////////////////////////////////////////////////////////////////
// Some internal functions.

// Get window version.
static int ACGetMiniWindowVers()
{
	OSVERSIONINFO osvi;
	osvi.dwOSVersionInfoSize = sizeof(osvi);
	::GetVersionEx (&osvi);
	if (VER_PLATFORM_WIN32_NT == osvi.dwPlatformId)
	{
		if (osvi.dwMajorVersion > 6)									    // win8
		{
			return 70;
		}
		else if ((6 == osvi.dwMajorVersion) && (osvi.dwMinorVersion >= 1))	// win7 // 2008 rc
		{
			return 61;
		}
		else if ((6 == osvi.dwMajorVersion) && (osvi.dwMinorVersion == 0))	// vista // 2008 rc1
		{
			return 60;
		}
		else if ((5 == osvi.dwMajorVersion) && (osvi.dwMinorVersion >= 1))	// xp  // 2003
		{
			return 51;
		}
		else if ((5 == osvi.dwMajorVersion) && (osvi.dwMinorVersion == 0))	// 2000
		{
			return 50;
		}
	}

	return 40;
}

//////////////////////////////////////////////////////////////////////////
// Get working area.

// Monitor type.
#define MONITOR_CENTER   0x0001        // center rect to monitor
#define MONITOR_CLIP     0x0000        // clip rect to monitor
#define MONITOR_WORKAREA 0x0002        // use monitor work area
#define MONITOR_AREA     0x0000        // use monitor entire area

// Get the working area rect.
static void GetWorkArea(LPRECT pRc)
{
	HMONITOR    hMonitor;
	MONITORINFO mi;
	RECT        rc;
	int         w = pRc->right  - pRc->left;
	int         h = pRc->bottom - pRc->top;

	hMonitor = MonitorFromRect(pRc, MONITOR_DEFAULTTONEAREST);
	mi.cbSize = sizeof(mi);
	GetMonitorInfo(hMonitor, &mi);
	rc = mi.rcWork;
	pRc->left   = max(rc.left, min(rc.right - w,  pRc->left));
	pRc->top    = max(rc.top,  min(rc.bottom - h, pRc->top));
	pRc->right  = pRc->left + w;
	pRc->bottom = pRc->top  + h;
}

//////////////////////////////////////////////////////////////////////////
// Another version of GetWorkArea with return value.
static CRect GetWorkAreaEx(LPCRECT rc)
{
	GetWorkArea((LPRECT)rc);
	return rc;
}

//////////////////////////////////////////////////////////////////////////
// Remove the paint message.
void DoPaintMessageLoop()
{
	MSG message1;
	while (::PeekMessage(&message1, NULL, WM_PAINT, WM_PAINT, PM_REMOVE))
	{
		::TranslateMessage(&message1);
		::DispatchMessage(&message1);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CACListScrollBar

CACListScrollBar::CACListScrollBar()
{
	
}

CACListScrollBar::~CACListScrollBar()
{
	
}

BEGIN_MESSAGE_MAP(CACListScrollBar, CScrollBar)
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_MESSAGE(WM_USER_ACLISTWND_MESSAGE_SETCAPTURE, OnSetCapture)
	ON_MESSAGE(WM_USER_ACLISTWND_MESSAGE_RELEASECAPTURES, OnReleaseCapture)
END_MESSAGE_MAP()

void CACListScrollBar::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO:
	// Release capture if moving out-side.
	CRect rcScroll;
	GetClientRect(&rcScroll);
	if (!rcScroll.PtInRect(point))
	{
		ReleaseCapture();
		GetParent()->SendMessage(WM_USER_ACLISTWND_MESSAGE_SETCAPTURE);
	}

	CScrollBar::OnLButtonDown(nFlags, point);
}

void CACListScrollBar::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO:
	// Set capture if lost capture when moving inside.
	CWnd* pCapWnd = GetCapture();
	if (pCapWnd != this)
	{
		if (NULL != pCapWnd)
		{
			ReleaseCapture();
		}

		SetCapture();
	}

	CRect rcScroll;
	GetClientRect(&rcScroll);
	if (!rcScroll.PtInRect(point))
	{
		ReleaseCapture();
		GetParent()->SendMessage(WM_USER_ACLISTWND_MESSAGE_SETCAPTURE);
	}

	CScrollBar::OnMouseMove(nFlags, point);
}

LRESULT CACListScrollBar::OnSetCapture(WPARAM wParam, LPARAM lParam)
{
	SetCapture();
	return S_OK;
}

LRESULT CACListScrollBar::OnReleaseCapture(WPARAM wParam, LPARAM lParam)
{
	ReleaseCapture();
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// CACListWnd

/**********************************************************************/
CACListWnd::CACListWnd()
{
    m_pSearchList       = new CACListSearchList;
    m_pDisplayList      = new CACListDisplayList;
    m_pSearchNodeAdder  = new CACListSearchNode;
    m_pDisplayNodeAdder = new CACListDisplayNode;

	m_lTopIndex     = 0;
	m_lCount        = 0;
	m_lItemHeight   = 16;
	m_lSelItem      = -1;
	m_lVisibleItems = 0;
	m_nKeywordsLen  = 0;
	m_pEditParent   = NULL;

	m_pCursorSourceID  = IDC_ARROW;
	m_bResizing        = FALSE;
	m_nMouseDiffX      = 0;
	m_nMouseDiffY      = 0;
	m_bRememberNewSize = FALSE;

	m_rcLastSize.SetRectEmpty();

	m_cPrefixChar     = 0;
	m_bListIsDisabled = FALSE;
	m_yMaxDisplayItem = 10;

    m_bSortDisplayList       = FALSE;
    m_bSortDisplayListAscend = TRUE;

	// Fix 1.7 Vista-Border
	m_SM_CXBORDER = GetSystemMetrics(SM_CXBORDER);	// 1
	m_SM_CYBORDER = GetSystemMetrics(SM_CYBORDER);	// 1

    m_clrNormalItemsBK         = ::GetSysColor(COLOR_WINDOW);
	m_clrEvenItemsBK           = RGB(235, 235, 235);
	m_clrNonSelItemsKeywordsBK = RGB(255, 255, 0);
    m_clrNormalItemsText       = ::GetSysColor(COLOR_WINDOWTEXT);
    
    m_clrSelItemBK   = ::GetSysColor(COLOR_HIGHLIGHT);
    m_clrSelItemText = ::GetSysColor(COLOR_HIGHLIGHTTEXT);

    // Reduce the brightness from m_clrSelectionBK for default.
    float fHue        = 0.0f;
    float fSaturation = 0.0f;
    float fBrightness = 0.0f;
    ColorSpaceRGB2HSB(GetRValue(m_clrSelItemBK), GetGValue(m_clrSelItemBK), GetBValue(m_clrSelItemBK), fHue, fSaturation, fBrightness);
    fBrightness -= 0.125f;

    BYTE yRed   = 0;
    BYTE yGreen = 0;
    BYTE yBlue  = 0;
    ColorSpaceHSB2RGB(fHue, fSaturation, fBrightness, yRed, yGreen, yBlue);
    m_clrSelItemKeywordsBK = RGB(yRed, yGreen, yBlue);
}

/**********************************************************************/
CACListWnd::~CACListWnd()
{
    if (NULL != m_pSearchList)
    {
        delete m_pSearchList;
        m_pSearchList = NULL;
    }

    if (NULL != m_pDisplayList)
    {
        delete m_pDisplayList;
        m_pDisplayList = NULL;
    }

    if (NULL != m_pSearchNodeAdder)
    {
        delete m_pSearchNodeAdder;
        m_pSearchNodeAdder = NULL;
    }

    if (NULL != m_pDisplayNodeAdder)
    {
        delete m_pDisplayNodeAdder;
        m_pDisplayNodeAdder = NULL;
    }

	DestroyWindow();
}

/**********************************************************************/
BEGIN_MESSAGE_MAP(CACListWnd, CWnd)
	//{{AFX_MSG_MAP(CACListWnd)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_KEYDOWN()
	ON_WM_NCCALCSIZE()
	ON_WM_VSCROLL()
	ON_WM_ACTIVATEAPP()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONDOWN()
	ON_WM_SETCURSOR()
	ON_WM_SHOWWINDOW()
	ON_WM_NCLBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_GETMINMAXINFO()
	ON_WM_NCHITTEST()
	ON_WM_MOUSEWHEEL()
	ON_MESSAGE(WM_USER_ACLISTWND_MESSAGE_SETCAPTURE, OnSetCapture)
	ON_MESSAGE(WM_USER_ACLISTWND_MESSAGE_RELEASECAPTURES, OnReleaseCapture)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/*********************************************************************/
void CACListWnd::OnActivateApp(BOOL bActive, DWORD dwThreadID) 
{
#if (_MSC_VER >= 1300)
	CWnd::OnActivateApp(bActive, dwThreadID);  //vc7    FIX 1.2
#else
	CWnd::OnActivateApp(bActive, (HTASK)dwThreadID); //vc6        FIX 1.2
#endif

	ShowWindow(SW_HIDE);
}

/////////////////////////////////////////////////////////////////////////////
// Behandlungsroutinen Nachrichten CACListWnd 

// Fix: 1.7 (Vista-Border)
void CACListWnd::DrawItem(CDC* pDC, long lItem, long lWidth)
{
	long lItemIdx   = lItem - m_lTopIndex;
	long lTxtOffset = m_SM_CXBORDER + 2;
	CRect rcLabel(
		m_SM_CXBORDER + 2, 
		(lItemIdx * m_lItemHeight) + m_SM_CYBORDER,
		lWidth,
		((lItemIdx + 1) * m_lItemHeight) + m_SM_CYBORDER
		);

    if (lItem == m_lSelItem)
    {
        rcLabel.left = m_SM_CXBORDER;
        pDC->FillSolidRect(rcLabel, m_clrSelItemBK);
        pDC->SetTextColor(m_clrSelItemText);
        rcLabel.left = lTxtOffset;
    }
	else if (lItem % 2 != 0)
	{
		rcLabel.left = m_SM_CXBORDER;
		pDC->FillSolidRect(rcLabel, m_clrEvenItemsBK);
        pDC->SetTextColor(m_clrNormalItemsText);
		rcLabel.left = lTxtOffset;
	}
    else
    {
        pDC->FillSolidRect(rcLabel, m_clrNormalItemsBK);
        pDC->SetTextColor(m_clrNormalItemsText);
    }

	if (m_cPrefixChar)
	{
		m_strDisplay = m_cPrefixChar + (*m_pDisplayList)[lItem]->first;
	}
	else
	{
		m_strDisplay = (*m_pDisplayList)[lItem]->first;
	}

	// Draw hight-light rect for the keywords.
    COLORREF clrKeywords = (lItem != m_lSelItem) ? m_clrNonSelItemsKeywordsBK : m_clrSelItemKeywordsBK;
    RECT     rcKeywords  = rcLabel;
    CSize    sizeLeft;
    CSize    sizeRight;

    CACListByteVector& aKeywordIdx = (*m_pDisplayList)[lItem]->third;
    for (CACListByteVector::const_iterator it = aKeywordIdx.begin(); it != aKeywordIdx.end(); ++it)
    {
        sizeLeft  = pDC->GetTextExtent((LPCTSTR)m_strDisplay, *it);
        sizeRight = pDC->GetTextExtent((LPCTSTR)m_strDisplay + *it, m_nKeywordsLen);

        rcKeywords.left   = rcLabel.left    + sizeLeft.cx;
        rcKeywords.right  = rcKeywords.left + sizeRight.cx;
        rcKeywords.top    = rcLabel.top     + 1;
        rcKeywords.bottom = rcLabel.bottom  - 1;
        pDC->FillSolidRect(&rcKeywords, clrKeywords);
    }

	// Fix: 1.10 CP: Yogurt
	UINT format = DT_SINGLELINE | DT_NOPREFIX | DT_VCENTER | DT_END_ELLIPSIS;
	if ((::GetWindowLong (m_hWnd, GWL_EXSTYLE) & (WS_EX_RIGHT | WS_EX_RTLREADING)) != 0)
	{
		format |= DT_RIGHT;
	}
	else
	{
		format |= DT_LEFT;
	}

	pDC->DrawText(m_strDisplay, -1, rcLabel, format);
}

/*********************************************************************/
// Fix: 1.7 (Vista-Border)
void CACListWnd::OnPaint() 
{
	CPaintDC dc(this);

	CRect rcWnd;
	CRect rcDraw;
	CRect rcGripper;

	CDC  dcMem;
	CDC* pDC = NULL;

	CBitmap  bitmapDraw;
	CBitmap* m_pOldBitmap;

	GetClientRect(rcGripper);
	rcWnd = rcDraw = rcGripper;

	rcGripper.left = rcGripper.right  - GetSystemMetrics(SM_CXVSCROLL);
	rcGripper.top  = rcGripper.bottom - GetSystemMetrics(SM_CYHSCROLL);
	
	rcDraw.right -= (GetScrollBarWidth() - m_SM_CXBORDER);
	
	dcMem.CreateCompatibleDC(&dc);
	
	bitmapDraw.CreateCompatibleBitmap(&dc, rcDraw.Width(), rcDraw.Height());
	m_pOldBitmap = dcMem.SelectObject(&bitmapDraw);

	dcMem.SetWindowOrg(rcDraw.left, rcDraw.top);

	long lWidth = rcWnd.Width() - GetScrollBarWidth();

	dcMem.FillSolidRect(rcWnd,::GetSysColor(COLOR_WINDOW));
	dcMem.SelectObject(GetStockObject(DEFAULT_GUI_FONT)); 
	dcMem.SetBkMode(TRANSPARENT);

	// (i) 1.5 vc2005
	int nVisibleMax = m_lTopIndex + m_lVisibleItems;
	nVisibleMax = (nVisibleMax > m_lCount) ? m_lCount : nVisibleMax;
	for (int i = m_lTopIndex; i < nVisibleMax; i++)
	{
		DrawItem(&dcMem, i, lWidth);
	}
	
	CPen m_Pen1(PS_SOLID, 1, ::GetSysColor(COLOR_WINDOW));
	CPen m_Pen2(PS_SOLID, 1, ::GetSysColor(COLOR_BTNFACE));
	CPen m_Pen3(PS_SOLID, 1, ::GetSysColor(COLOR_3DSHADOW));
	CPen m_Pen4(PS_SOLID, 1, ::GetSysColor(COLOR_WINDOWFRAME));

	pDC = &dc;

	if (m_wndVertBar.IsWindowVisible())
	{
		dc.FillSolidRect(rcGripper, ::GetSysColor(COLOR_BTNFACE));
	}
	else
	{
		pDC = &dcMem;
	}

	CPen* pOldPen = (CPen*)pDC->SelectObject(&m_Pen1);
	rcGripper.OffsetRect(-1, 1);

	lWidth = GetSystemMetrics(SM_CXVSCROLL);
	int nBottom = (rcWnd.bottom - GetSystemMetrics(SM_CYHSCROLL)) - m_SM_CYBORDER;

	// gripper
	// (i) 1.5 vc2005
	for (int i = 0, a = 1; i < 20 ; i++, a++)
	{
		if (1 == a)
		{
			pDC->SelectObject(&m_Pen1);
		}

		if (2 == a)
		{
			pDC->SelectObject(&m_Pen2);
		}

		if (3 == a)
		{
			pDC->SelectObject(&m_Pen3);
		}

		if (3 == a)
		{
			a = 0;
		}
	
		pDC->MoveTo(rcGripper.left + i - 1, rcWnd.bottom);
		pDC->LineTo(rcGripper.left + i + lWidth, nBottom);
	}

	// Fix: 1.7 (Vista-Border)
	dcMem.SelectObject(&m_Pen4);
	dcMem.MoveTo(rcWnd.left, rcWnd.top);
	dcMem.LineTo(rcWnd.right-1, rcWnd.top);
	dcMem.LineTo(rcWnd.right-1, rcWnd.bottom - 1);
	dcMem.LineTo(0, rcWnd.bottom - 1);
	dcMem.LineTo(0, 0);

	if (m_wndVertBar.IsWindowVisible())
	{
		pDC->SelectObject(&m_Pen4);
		pDC->MoveTo(rcWnd.right - 30, rcWnd.top);
		pDC->LineTo(rcWnd.right - 1, rcWnd.top);
		pDC->LineTo(rcWnd.right - 1, rcWnd.bottom - 1);
		pDC->LineTo(rcWnd.right - 30, rcWnd.bottom - 1);
	}

	dc.BitBlt(rcDraw.left, rcDraw.top, rcDraw.Width(), rcDraw.Height(), &dcMem, rcDraw.left, rcDraw.top, SRCCOPY);

	pDC->SelectObject(pOldPen);
	dcMem.SelectObject(m_pOldBitmap);
}

/*********************************************************************/
BOOL CACListWnd::CreateAndInit(CWnd* pParentWnd)
{
	if (NULL == pParentWnd)
	{
		ASSERT(0);
		return FALSE;
	}

	m_pEditParent = (CEdit*)pParentWnd;

	// Create list window first.

	// Get windows version.
	DWORD dwClassNameFlags = CS_DROPSHADOW;
	if (ACGetMiniWindowVers() < 51)
	{
		dwClassNameFlags = 0;
	}

	// Get class name.
	CString szClassName = AfxRegisterWndClass(
		CS_CLASSDC | CS_SAVEBITS | CS_HREDRAW | CS_VREDRAW | dwClassNameFlags, 0, (HBRUSH)(COLOR_WINDOW), 0
		);
	
	CRect rcParentWnd;
	pParentWnd->GetWindowRect(rcParentWnd);
	BOOL bCreateSus = CreateEx(
		WS_EX_TOOLWINDOW, szClassName, NULL,
		WS_THICKFRAME | WS_CHILD | WS_BORDER | WS_CLIPSIBLINGS | WS_OVERLAPPED,
		CRect(rcParentWnd.left, rcParentWnd.top + 20, rcParentWnd.left + 200, rcParentWnd.top + 200),
		GetDesktopWindow(), 
		0x3E8, NULL
		);
	if (!bCreateSus)
	{
		ASSERT(0);
		return FALSE;
	}

	// Create scroll bar.
	VERIFY(m_wndVertBar.Create(
		WS_VISIBLE | SBS_VERT | SBS_LEFTALIGN,
		CRect(0, 0, GetSystemMetrics(SM_CYVSCROLL), 100), this, 0
		));
	
	SetScroller();

	m_lCount = (long)(*m_pDisplayList)->size();
	m_wndVertBar.SetScrollPos(0, FALSE);
	SetProp();

	// Get item height.
	CDC* pDC = GetDC();
	if (NULL != pDC)
	{
		HGDIOBJ hOldFont = pDC->SelectObject(GetStockObject(DEFAULT_GUI_FONT));
		CSize   cSize    = pDC->GetOutputTextExtent(_T("ÔÚg"));

		m_lItemHeight = cSize.cy;

		long lMinItemHeight = GetSystemMetrics(SM_CYHSCROLL) - m_SM_CYBORDER;
		m_lItemHeight = (m_lItemHeight < lMinItemHeight) ? lMinItemHeight : m_lItemHeight;

		pDC->SelectObject(hOldFont);
		ReleaseDC(pDC);
	}

	return TRUE;
}

/*********************************************************************/
void CACListWnd::AddSearchString(LPCTSTR lpszString, DWORD dwData)
{
    m_pSearchNodeAdder->SetNode(lpszString, dwData);
    (*m_pSearchList)->push_back(*m_pSearchNodeAdder);
}

/*********************************************************************/
void CACListWnd::RemoveAt(int nItem)
{
	if ((nItem < 0) || (nItem >= GetSourcesCount()))
	{
		return;
	}

	(*m_pSearchList)->erase((*m_pSearchList)->begin() + nItem);
}

/*********************************************************************/
void CACListWnd::RemoveAll()
{
	(*m_pSearchList)->clear();
    m_pDisplayList->Clear();
}

/*********************************************************************/
int CACListWnd::GetSourcesCount() const
{
    return (int)(*m_pSearchList)->size();
}

/*********************************************************************/
BYTE CACListWnd::SetMaxDisplayItem(BYTE yItem)
{
	if (yItem > 0)
	{
        BYTE yOldItem = m_yMaxDisplayItem;
		m_yMaxDisplayItem = yItem;

        return yOldItem;
	}
    else
    {
        return yItem;
    }
}

/*********************************************************************/
BOOL CACListWnd::DisabePopup(BOOL bDisable)
{
    BOOL bOldDisable = m_bListIsDisabled;
	m_bListIsDisabled = bDisable;

    return bOldDisable;
}

/*********************************************************************/
void CACListWnd::SetScroller()
{
	CRect rcWnd, rcBar;
	GetClientRect(rcWnd);

	if (m_wndVertBar.GetSafeHwnd())
	{
		rcBar         = rcWnd;	
		rcBar.left    = (rcWnd.Width() - GetSystemMetrics(SM_CXVSCROLL));
		rcBar.right   = rcBar.left + GetSystemMetrics(SM_CXVSCROLL) - m_SM_CXBORDER;
		rcBar.top     = m_SM_CYBORDER;
		rcBar.bottom -= GetSystemMetrics(SM_CYHSCROLL);

		m_wndVertBar.MoveWindow(rcBar);
		m_wndVertBar.SetScrollPos(m_lTopIndex, TRUE);	
	}	
}

/*********************************************************************/
void CACListWnd::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);
	OnChangeWindowSize();
}

/*********************************************************************/
long CACListWnd::GetScrollBarWidth()
{
	if (m_wndVertBar.IsWindowVisible())
	{
		return GetSystemMetrics(SM_CXVSCROLL);
	}
	else
	{
		return 0;
	}
}

/*********************************************************************/
void CACListWnd::SetProp()
{
	CRect rcWnd, rcBar;

	if (0 == m_lCount)
	{
		return;
	}

	CWnd::GetWindowRect(rcWnd);
	ScreenToClient(rcWnd);

	SCROLLINFO si;
	si.cbSize = sizeof(SCROLLINFO);
	si.fMask  = SIF_PAGE | SIF_RANGE;
	si.nMin   = 0;
	si.nMax   = m_lCount - 1;
	
	// Fix: 1.8 /  -4 = top/bottom-border
	// ist hscher ;-)
    m_lVisibleItems = (m_lCount < m_yMaxDisplayItem) ? m_lCount : m_yMaxDisplayItem;
	if (!m_rcLastSize.IsRectEmpty())
	{
		// If the user resize the windows, the visible items will be changed.
		long lListHeight = m_rcLastSize.Height() - m_SM_CYBORDER * 2;
		m_lVisibleItems = (0 == (lListHeight % m_lItemHeight)) ? (lListHeight / m_lItemHeight) : ((lListHeight + m_lItemHeight) / m_lItemHeight);
	}

	si.nPage     = m_lVisibleItems;
	si.nTrackPos = 2;

	m_wndVertBar.SetScrollRange(0, m_lCount - 1);
	m_wndVertBar.SetScrollInfo(&si);

	if (m_lVisibleItems >= m_lCount)
	{
		m_wndVertBar.ShowWindow(SW_HIDE);
	}
	else if (!m_bListIsDisabled)
	{
		m_wndVertBar.ShowWindow(SW_SHOWNORMAL);
	}

	if (m_lTopIndex + m_lVisibleItems > m_lCount)
	{
		m_lTopIndex = m_lCount - m_lVisibleItems;
		if (m_lTopIndex < 0)
		{
			m_lTopIndex = 0;
		}

		m_wndVertBar.SetScrollPos(m_lTopIndex, TRUE);
	}
}

/*********************************************************************/
void CACListWnd::OnChangeWindowSize()
{
	SetScroller();
	SetProp();

	if (!m_rcLastSize.IsRectEmpty())
	{
		GetWindowRect(m_rcLastSize);
	}
}

/*********************************************************************/
BOOL CACListWnd::OnEraseBkgnd(CDC* pDC) 
{
	return FALSE;
}

/*********************************************************************/
// Fix: 1.7 (Vista-Border)
//void CACListWnd::OnNcPaint() 
//{
//	CWindowDC dc(this);
//	CRect rectClient, rectWindow,rcWnd;
//
//	GetClientRect(rectClient);
//	GetWindowRect(rectWindow);
//	ScreenToClient(rectWindow);
//	
//	rectClient.OffsetRect(-(rectWindow.left), -(rectWindow.top));
//	dc.ExcludeClipRect(rectClient);
//
//	rectWindow.OffsetRect(-rectWindow.left, -rectWindow.top);
//
//	dc.FillSolidRect(rectWindow,::GetSysColor(COLOR_WINDOWTEXT));
//}

/*********************************************************************/
void CACListWnd::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	CWnd::OnKeyDown(nChar, nRepCnt, nFlags);

	if (VK_ESCAPE == nChar)
	{
		ShowWindow(SW_HIDE);
	}
}

/*********************************************************************/
// Fix: 1.7 (Vista-Border) / V.10
void CACListWnd::OnNcCalcSize(BOOL /*bCalcValidRects*/, NCCALCSIZE_PARAMS FAR* /*lpncsp*/) 
{
	//::InflateRect(lpncsp->rgrc, -0, -0);
}

/*********************************************************************/

int CACListWnd::HitTest(CPoint point)
{
	CRect rcItem;
	CRect rcWnd;

	GetClientRect(rcWnd);
	long width = rcWnd.Width() - GetScrollBarWidth();

	int nVisibleMax = m_lTopIndex + m_lVisibleItems;
	nVisibleMax = (nVisibleMax > m_lCount) ? m_lCount : nVisibleMax;
	for (int i = m_lTopIndex; i < nVisibleMax; i++)
	{
		long y = i - m_lTopIndex;
		rcItem.SetRect(2, y * m_lItemHeight,width, (y + 1) * m_lItemHeight);

		if (PtInRect(&rcItem, point))
		{
			return (m_lSelItem = (y + m_lTopIndex));
		}
	}

	return -1;
}

/*********************************************************************/
#if _MFC_VER < 0x0800  // Fix: 1.5
UINT CACListWnd::OnNcHitTest(CPoint point) 
#else
LRESULT CACListWnd::OnNcHitTest(CPoint point) 
#endif
{
	CRect rectClient;
	GetWindowRect(rectClient);

	rectClient.left = rectClient.right - GetSystemMetrics(SM_CXVSCROLL);
	rectClient.top  = rectClient.bottom - GetSystemMetrics(SM_CYHSCROLL);

	if (rectClient.PtInRect(point))
	{
		return HTBOTTOMRIGHT;
	}
	else
	{
		return HTCLIENT;
	}
}

/*********************************************************************/
void CACListWnd::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// Windos area.
	CRect rcWnd;
	GetClientRect(&rcWnd);

	// Size bar area.
	CRect rcSizebar;
	rcSizebar.left   = rcWnd.Width() - GetSystemMetrics(SM_CYVSCROLL);
	rcSizebar.right  = rcSizebar.left + GetSystemMetrics(SM_CYVSCROLL) - m_SM_CXBORDER;
	rcSizebar.top    = rcWnd.bottom - GetSystemMetrics(SM_CYHSCROLL);
	rcSizebar.bottom = rcSizebar.top + GetSystemMetrics(SM_CYHSCROLL) - m_SM_CYBORDER;

	if (rcSizebar.PtInRect(point))
	{
		// Tag that we gonna be resizing.
		m_bResizing = TRUE;

		// Remember the mouse position.
		CRect rcWnd;
		GetClientRect(&rcWnd);
		m_nMouseDiffX = rcWnd.Width() - point.x;
		m_nMouseDiffY = rcWnd.Height() - point.y;

		// Remember the last window size.
		GetWindowRect(m_rcLastSize);

		return;
	}

	// Hit test
	int sel = HitTest(point);
	if (sel >= 0)
	{
		if (!EnsureVisible(sel, TRUE))
		{
			Invalidate();
		}

		m_lSelItem = sel;
		m_pEditParent->SendMessage(WM_USER_ACLISTWND_MESSAGE_CLICK_LIST, 0, 0);
		DoPaintMessageLoop();
		//Sleep(500);
		ShowWindow(SW_HIDE);
	}
	else if (!rcWnd.PtInRect(point))
	{
		ShowWindow(SW_HIDE);
	}

	CWnd::OnLButtonDown(nFlags, point);
}

/*********************************************************************/
void CACListWnd::OnLButtonUp(UINT nFlags, CPoint point)
{
	m_bResizing = FALSE;
	if (!m_bRememberNewSize)
	{
		m_rcLastSize.SetRectEmpty();
	}

	CWnd::OnLButtonUp(nFlags, point);
}

/*********************************************************************/
void CACListWnd::OnRButtonDown(UINT nFlags, CPoint point) 
{
	ShowWindow(SW_HIDE);
	CWnd::OnRButtonDown(nFlags, point);
}

/*********************************************************************/
BOOL CACListWnd::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	CRect rectClient;
	CPoint ptCursor(GetMessagePos());
	ScreenToClient(&ptCursor);

	GetWindowRect(rectClient);
	ScreenToClient(&rectClient);

	rectClient.left = rectClient.right - GetSystemMetrics(SM_CXVSCROLL);
	rectClient.top  = rectClient.bottom - GetSystemMetrics(SM_CYHSCROLL);

	if (rectClient.PtInRect(ptCursor)) // Resize-Cursor
	{
		return CWnd::OnSetCursor(pWnd, nHitTest, message);
	}
	
	::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
	return TRUE;		
}

/*********************************************************************/
void CACListWnd::InvalidateAndScroll()
{
	m_wndVertBar.SetScrollPos(m_lTopIndex, TRUE);
	Invalidate();
	DoPaintMessageLoop();
}

/*********************************************************************/
BOOL CACListWnd::EnsureVisible(int nItem, BOOL bWait)
{
	if ((nItem > m_lTopIndex) && (nItem < m_lTopIndex + m_lVisibleItems))
	{
		return FALSE; // It is already visible.
	}

	// Scroll down.
	if (nItem > m_lTopIndex)
	{
		long lLen = nItem;
		for (long i = m_lTopIndex; i < lLen; i++)
		{
			if (i >= m_lCount - m_lVisibleItems)
			{
				break;
			}

			if ((i >= m_lCount - m_lVisibleItems) || (i + m_lVisibleItems > nItem))
			{
				break;
			}

			m_lTopIndex++;

			if (bWait)
			{
				InvalidateAndScroll();
				Sleep(10);
				DoPaintMessageLoop();
			}
		}

		InvalidateAndScroll();
		return TRUE;
	}

	// Scroll up.
	if (nItem < m_lTopIndex)
	{
		while(nItem < m_lTopIndex)
		{
			if (m_lTopIndex > 0)
			{
				m_lTopIndex--;
			}
			else
			{
				break;
			}

			if (bWait)
			{
				InvalidateAndScroll();
				Sleep(10);
				DoPaintMessageLoop();
			}
		}
		
		InvalidateAndScroll();
		return TRUE;
	}

	return FALSE;
}

/*********************************************************************/
BOOL CACListWnd::SelectItem(int nItem)
{
	if (nItem > m_lCount)
	{
		return FALSE;
	}

	if (-1 == nItem)
	{
		EnsureVisible(m_lSelItem, FALSE);
		Invalidate();

		return FALSE;
	}

	m_lSelItem = nItem;

	if (!EnsureVisible(nItem, TRUE))
	{
		Invalidate();
	}

	return TRUE;
}

/*********************************************************************/
int CACListWnd::GetResultsCount() const
{
    return m_lCount;
}

/*********************************************************************/
int CACListWnd::FindStringExact(LPCTSTR lpszString, int nStartAfter)
{
    int nSourcesCount = GetSourcesCount();
	if (nStartAfter > nSourcesCount)
	{
		return -1;
	}

	for (int i = nStartAfter + 1; i < nSourcesCount; i++)
	{
		if (0 == (*m_pSearchList)[i].first.Compare(lpszString))
		{
			return i;
		}
	}

	return -1;
}

/*********************************************************************/
void CACListWnd::AdjustListPosition(CRect& rcWnd)
{
    // Make sure the window is in the work area.

	CRect rectScreen;
	::GetWindowRect(::GetDesktopWindow(), &rectScreen);

	rectScreen = GetWorkAreaEx(rcWnd); // fix 1.6

	if (rcWnd.right > rectScreen.right)
	{
		rcWnd.OffsetRect(-(rcWnd.right - rectScreen.right), 0);
	}

	if (rcWnd.bottom > rectScreen.bottom)
	{
		rcWnd.OffsetRect(0, -(rcWnd.bottom - rectScreen.bottom));
	}

    // Make sure the window is align the top of parent if the 
    // height is large than the height which is the subtract of
    // the bottom of work area and the top of the parent window.
    CRect rcParent;
    ::GetWindowRect(m_pEditParent->GetSafeHwnd(), rcParent);
    if (rcParent.bottom > rcWnd.top)
    {
        rcWnd.OffsetRect(0, -(rcWnd.bottom - rcParent.top));
        rcWnd.top = (rcWnd.top < 0) ? 0 : rcWnd.top;
    }
}

/**********************************************************************/
/*
** Vers. 1.1
* NEW: bDisplayOnly
*/
int CACListWnd::FindString(LPCTSTR lpszString, BOOL bCaseSense, BOOL bMatchAny, int nStartAfter, BOOL bDisplayOnly)
{
	if (!bDisplayOnly)
	{
		CString m_Str1;
		CString m_Str2 = lpszString;

		m_nKeywordsLen = m_Str2.GetLength();

		if (NULL == m_pEditParent)
		{
			ShowWindow(SW_HIDE);
			return -1;
		}

        int nSourcesCount = GetSourcesCount();
		if (nStartAfter >= nSourcesCount)
		{
			ShowWindow(SW_HIDE);
			return -1;
		}

		if (m_Str2.IsEmpty())
		{
			ShowWindow(SW_HIDE);
			return -1;
		}

        m_lSelItem = -1;
		m_pDisplayList->Clear();

		// Fix: 1.10 CP: Yogurt
		if (!bCaseSense)
		{
			m_Str2.MakeLower();
		}

		for (int i = nStartAfter + 1; i < nSourcesCount; i++)
		{
			if (m_cPrefixChar)
			{
				m_Str1 = m_cPrefixChar;
			}
			else
			{
				m_Str1.Empty();
			}

			m_Str1 += (*m_pSearchList)[i].first;

			// Fix: 1.10 CP: Yogurt
			// Case sensitive or not.
			if (!bCaseSense)
			{
				m_Str1.MakeLower();
			}

			// Match key words anywhere or not.
			if (!bMatchAny)
			{
				// Fix: 1.10 CP: Yogurt
				// Just matches any keywords from the beginning.
				if (0 == _tcsncmp(m_Str1, m_Str2, m_Str2.GetLength()))
				{
                    m_pDisplayNodeAdder->Clear();
                    m_pDisplayList->GetRawData().push_back(*m_pDisplayNodeAdder);

                    CACListDisplayNode* pNode = &(m_pDisplayList->GetRawData().back());
                    pNode->SetBaseNode((*m_pSearchList)[i]);
                    pNode->third.push_back(0);

                    (*m_pDisplayList)->push_back(pNode);
				}
			}
			else
			{
				// Just matches any keywords anywhere.
				int nIdx = m_Str1.Find(m_Str2);
				if (-1 != nIdx)
				{
                    m_pDisplayNodeAdder->Clear();
                    m_pDisplayList->GetRawData().push_back(*m_pDisplayNodeAdder);
                    
                    CACListDisplayNode* pNode = &(m_pDisplayList->GetRawData().back());
                    pNode->SetBaseNode((*m_pSearchList)[i]);

					while (-1 != nIdx)
					{
						pNode->third.push_back((BYTE)nIdx);
						nIdx = m_Str1.Find(m_Str2, nIdx + m_nKeywordsLen);
					}

                    (*m_pDisplayList)->push_back(pNode);
				}
			}
		}
	}

	m_lCount = (long)(*m_pDisplayList)->size();
	if (m_lCount > 0)
	{
		CRect rcWnd;
		m_pEditParent->GetWindowRect(rcWnd);
		
		SetScroller();
		SetProp();
		
		if (!m_bListIsDisabled)
		{
			ShowWindow(SW_SHOWNORMAL);
		}

		Invalidate();

		int nHeight = 0;
		int nWidth  = 0;
		if (m_lCount > m_yMaxDisplayItem)
		{
			nHeight = (m_yMaxDisplayItem * m_lItemHeight) + (m_SM_CYBORDER * 2);
		}
		else
		{
			nHeight = (m_lCount * m_lItemHeight) + (m_SM_CYBORDER * 2);
		}

		// V1.3
		if (!m_rcLastSize.IsRectEmpty())
		{
			nWidth  = m_rcLastSize.Width(); 
			nHeight = m_rcLastSize.Height(); 
		}
		else
		{
			nWidth = rcWnd.Width(); 
		}

		rcWnd.top    += rcWnd.Height();
		rcWnd.right   = rcWnd.left + nWidth;
		rcWnd.bottom  = rcWnd.top + nHeight;
		
		AdjustListPosition(rcWnd);
		SetWindowPos(&CWnd::wndTopMost, rcWnd.left, rcWnd.top, rcWnd.Width(), rcWnd.Height(), 0);
		
		// Sort the results or not.
        if (m_bSortDisplayList)
        {
            SortList(*m_pDisplayList);
        }
	}
	else
	{
		ShowWindow(SW_HIDE);
	}

	return m_lCount;	
}

/*********************************************************************/
int CACListWnd::SelectString(LPCTSTR lpszString)
{
	int item = FindString(lpszString, FALSE, FALSE);
	SelectItem(item);

	return item;
}

/*********************************************************************/
BOOL CACListWnd::GetSourceText(int nItem, CString& strText)
{
	if ((nItem < 0) || (nItem >= GetSourcesCount()))
	{
		return FALSE;
	}

	strText = (*m_pSearchList)[nItem].first;
	return TRUE;
}

/*********************************************************************/
DWORD CACListWnd::GetSourceData(int nItem)
{
	if ((nItem < 0) || (nItem >= GetSourcesCount()))
	{
		return FALSE;
	}

	return (*m_pSearchList)[nItem].second;
}

/*********************************************************************/
void CACListWnd::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	if (bShow)
	{
		SetCapture();
	}
	else
	{
		m_lTopIndex = 0;
		m_bResizing = FALSE;
		if (!m_bRememberNewSize)
		{
			m_rcLastSize.SetRectEmpty();
		}

		ReleaseCapture();
	}

	::SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
	CWnd::OnShowWindow(bShow, nStatus);
}

/*********************************************************************/
void CACListWnd::OnNcLButtonDown(UINT nHitTest, CPoint point) 
{
	if (OnNcHitTest(point) == HTBOTTOMRIGHT)
	{
		GetWindowRect(m_rcLastSize);
	}

	CWnd::OnNcLButtonDown(nHitTest, point);
}

/*********************************************************************/
LPCTSTR CACListWnd::GetSelectedString() const
{
	int i = (int)m_lCount;
	if (0 == i)
	{
		return NULL;
	}

	if ((i <= m_lSelItem) || (-1 == m_lSelItem))
	{
		i = 0;
	}
	else
	{
		i = m_lSelItem;
	}

	return (*m_pDisplayList)[i]->first;
}

/*********************************************************************/
DWORD CACListWnd::GetSelectedData() const
{
	int i = (int)m_lCount;
	if (0 == i)
	{
		return UINT_MAX;
	}

	if ((i <= m_lSelItem) || (-1 == m_lSelItem))
	{
		i = 0;
	}
	else
	{
		i = m_lSelItem;
	}

	return (*m_pDisplayList)[i]->second;
}

/*********************************************************************/
void CACListWnd::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	CWnd::OnVScroll(nSBCode, nPos, pScrollBar);

	// Calculate the real position.
	long m_oldlTopIndex = m_lTopIndex;
	switch(nSBCode)
	{
		case SB_ENDSCROLL:
			{
				// Test if the cursor is in scroll bar range.

				// Get cursor position.
				CPoint ptCursor(GetMessagePos());
				ScreenToClient(&ptCursor);

				// Prepare to get some area we need.
				CRect rcWnd;
				GetClientRect(&rcWnd);
				CRect rcScroll;
				m_wndVertBar.GetClientRect(&rcScroll);

				// Scroll bar area.
				CRect rcScrollInList;
				rcScrollInList.left   = rcWnd.Width() - rcScroll.Width() - m_SM_CXBORDER;
				rcScrollInList.right  = rcScrollInList.left + rcScroll.Width();
				rcScrollInList.top    = m_SM_CYBORDER;
				rcScrollInList.bottom = rcScrollInList.top + rcScroll.Height();

				// Mouse cursor is not in the scroll bar area, just release the capture and give to list window.
				if (!rcScrollInList.PtInRect(ptCursor))
				{
					m_wndVertBar.SendMessage(WM_USER_ACLISTWND_MESSAGE_RELEASECAPTURES);
					SetCapture();
				}
			}
			break;
		case SB_PAGEUP:
			{
				m_lTopIndex -= m_lVisibleItems;
				if (m_lTopIndex < 0)
				{
					m_lTopIndex = 0;
				}
			}
			break;
		case SB_PAGEDOWN:
			{
				m_lTopIndex += m_lVisibleItems;
				if (m_lTopIndex >= m_lCount - m_lVisibleItems)
				{
					m_lTopIndex = m_lCount - m_lVisibleItems;
				}
			}
			break;
		case SB_LINEUP:
			{
				m_lTopIndex--;
				if (m_lTopIndex < 0)
				{
					m_lTopIndex = 0;
				}
			}
			break;
		case SB_LINEDOWN:
			{
				m_lTopIndex++;
				if (m_lTopIndex >= m_lCount - m_lVisibleItems)
				{
					m_lTopIndex = m_lCount - m_lVisibleItems;
				}
			}
			break;
		case SB_THUMBTRACK:
			{
				// Get the real scroll position info, as nPos may be negative.
				SCROLLINFO sf;
				m_wndVertBar.GetScrollInfo(&sf);
				m_lTopIndex = sf.nTrackPos;
				//m_lTopIndex = nPos;
			}
			break;
		default:
			break;
	}
	
	m_wndVertBar.SetScrollPos(m_lTopIndex, TRUE);

	if (m_oldlTopIndex != m_lTopIndex)
	{
		// Just redraw the items part, not including the scrollbar.
		CRect rc;
		GetClientRect(&rc);
		rc.right -= GetScrollBarWidth();
		InvalidateRect(&rc);
		//Invalidate();
	}
}

/*********************************************************************/
LPCTSTR CACListWnd::GetNextString(int nChar)
{
	switch(nChar)
	{
		case VK_DOWN:
			m_lSelItem++;
			break;
		case VK_UP:
			m_lSelItem--;
			break;
		case VK_PRIOR:
			{
				m_lSelItem -= m_lVisibleItems;
				if (m_lSelItem < 0)
				{
					m_lSelItem = 0;
				}
			}
			break;
		case VK_NEXT:
			{
				m_lSelItem += m_lVisibleItems;
				if (m_lSelItem >= m_lCount - 1)
				{
					m_lSelItem = m_lCount - 1;
				}
			}
			break;
		case VK_HOME:
			m_lSelItem = 0;
			break;
		case VK_END:
			m_lSelItem = m_lCount - 1;
			break;
		default:
			break;
	}

	if (m_lSelItem < 0)
	{
		m_lSelItem = m_lCount - 1;
	}

	if (m_lSelItem >= m_lCount)
	{
		m_lSelItem = 0;
	}

	if (EnsureVisible(m_lSelItem, (m_lCount > 50) ? FALSE : TRUE))
	{
		InvalidateAndScroll();
	}

	return GetSelectedString();
}

/*********************************************************************/
COLORREF CACListWnd::SetNormalItemsBKColor(COLORREF clrNormalItemsBK)
{
    COLORREF clrTemp = m_clrNormalItemsBK;
    m_clrNormalItemsBK = clrNormalItemsBK;

    return clrTemp;
}

/*********************************************************************/
COLORREF CACListWnd::SetEvenItemsBKColor(COLORREF clrEvenItemsBK)
{
	COLORREF clrTemp = m_clrEvenItemsBK;
	m_clrEvenItemsBK = clrEvenItemsBK;

	return clrTemp;
}

/*********************************************************************/
COLORREF CACListWnd::SetNonSelItemsKeywordsBKColor(COLORREF clrNonSelItemsKeywordsBK)
{
	COLORREF clrTemp = m_clrNonSelItemsKeywordsBK;
	m_clrNonSelItemsKeywordsBK = clrNonSelItemsKeywordsBK;

	return clrTemp;
}

/*********************************************************************/
COLORREF CACListWnd::SetNormalItemsTextColor(COLORREF clrNormalItemsText)
{
    COLORREF clrTemp = m_clrNormalItemsText;
    m_clrNormalItemsText = clrNormalItemsText;

    return clrTemp;
}

/*********************************************************************/
COLORREF CACListWnd::SetSelItemBKColor(COLORREF clrSelItemBK, BOOL bAutoSetSelKeywordsColor)
{
    COLORREF clrTemp = m_clrSelItemBK;
    m_clrSelItemBK = clrSelItemBK;

    if (bAutoSetSelKeywordsColor)
    {
        float fHue        = 0.0f;
        float fSaturation = 0.0f;
        float fBrightness = 0.0f;
        ColorSpaceRGB2HSB(GetRValue(m_clrSelItemBK), GetGValue(m_clrSelItemBK), GetBValue(m_clrSelItemBK), fHue, fSaturation, fBrightness);
        fBrightness -= 0.125f;

        BYTE yRed   = 0;
        BYTE yGreen = 0;
        BYTE yBlue  = 0;
        ColorSpaceHSB2RGB(fHue, fSaturation, fBrightness, yRed, yGreen, yBlue);
        SetSelItemKeywordsBKColor(RGB(yRed, yGreen, yBlue));
    }

    return clrTemp;
}

/*********************************************************************/
COLORREF CACListWnd::SetSelItemKeywordsBKColor(COLORREF clrSelItemKeywordsBK)
{
    COLORREF clrTemp = m_clrSelItemKeywordsBK;
    m_clrSelItemKeywordsBK = clrSelItemKeywordsBK;

    return clrTemp;
}

/*********************************************************************/
COLORREF CACListWnd::SetSelItemTextColor(COLORREF clrSelItemText)
{
    COLORREF clrTemp = m_clrSelItemText;
    m_clrSelItemText = clrSelItemText;

    return clrTemp;
}

/*********************************************************************/
BOOL CACListWnd::SetRememberWindowNewSize(BOOL bRemember)
{
	BOOL bOldRemember = m_bRememberNewSize;
	m_bRememberNewSize = bRemember;

	return bRemember;
}

/*********************************************************************/
BOOL CACListWnd::SetSortDisplayedSearchResults(BOOL bSort, BOOL bAscend)
{
    BOOL bOldSort = m_bSortDisplayList;
    m_bSortDisplayList       = bSort;
    m_bSortDisplayListAscend = bAscend;

    return bOldSort;
}

/*********************************************************************/
//V1.9
BOOL CACListWnd::DoMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	if (nFlags & (MK_SHIFT | MK_CONTROL))
	{
		return FALSE;
	}
	
	if (!m_wndVertBar.IsWindowVisible())
	{
		return FALSE;
	}
	
	BOOL bResult = TRUE;

	int nToScroll     = -1;
	int nDisplacement = -1;
	int nCurFirstLine = -1;

	nCurFirstLine = m_lTopIndex;
	nToScroll     = ::MulDiv(-zDelta, 1, WHEEL_DELTA);

	if (nToScroll > 0)
	{
		nDisplacement  = abs(nToScroll * (m_lVisibleItems / 4));
		m_lTopIndex   += nDisplacement;
		m_lTopIndex    = min(m_lTopIndex, m_lCount - m_lVisibleItems);
	}
	else
	{
		nDisplacement  = abs(nToScroll * (m_lVisibleItems / 4));
		m_lTopIndex   -= nDisplacement;
		m_lTopIndex    = max(0, m_lTopIndex);
	}
			
	if (nCurFirstLine != m_lTopIndex)
	{
		m_wndVertBar.SetScrollPos(m_lTopIndex, TRUE);
		ScreenToClient(&pt);
		m_lSelItem = HitTest(pt);
		CWnd::Invalidate();
	}

	return bResult;
}

/**********************************************************************/
void CACListWnd::OnMouseMove(UINT nFlags, CPoint point) 
{
	// Resizing window.
	if (m_bResizing)
	{
		CRect rcWnd;
		GetWindowRect(&rcWnd);

		rcWnd.right  = rcWnd.left + point.x + m_nMouseDiffX;
		rcWnd.bottom = rcWnd.top + point.y + m_nMouseDiffY;

		MoveWindow(&rcWnd);
		OnChangeWindowSize();

		return;
	}

	// Hit test.
	int nSel = HitTest(point);
	if (nSel >= 0)
	{
		Invalidate();
	}

	// Prepare to get some area we need.
	CRect rcWnd;
	GetClientRect(&rcWnd);
	CRect rcScroll;
	m_wndVertBar.GetClientRect(&rcScroll);

	// Scroll bar area.
	CRect rcScrollInList;
	rcScrollInList.left   = rcWnd.Width() - rcScroll.Width() - m_SM_CXBORDER;
	rcScrollInList.right  = rcScrollInList.left + rcScroll.Width();
	rcScrollInList.top    = m_SM_CYBORDER;
	rcScrollInList.bottom = rcScrollInList.top + rcScroll.Height();

	// Size bar area.
	CRect rcSizebar;
	rcSizebar.left   = rcWnd.Width() - GetSystemMetrics(SM_CXVSCROLL);
	rcSizebar.right  = rcSizebar.left + GetSystemMetrics(SM_CXVSCROLL) - m_SM_CXBORDER;
	rcSizebar.top    = rcWnd.bottom - GetSystemMetrics(SM_CYHSCROLL);
	rcSizebar.bottom = rcSizebar.top + GetSystemMetrics(SM_CYHSCROLL) - m_SM_CYBORDER;

	// Test area.
	if (rcSizebar.PtInRect(point))
	{
		// In size bar area.
		if (IDC_SIZENWSE != m_pCursorSourceID)
		{
			SetCursor(LoadCursor(NULL, IDC_SIZENWSE));
			m_pCursorSourceID = IDC_SIZENWSE;
		}
	}
	else if (rcScrollInList.PtInRect(point) && m_wndVertBar.IsWindowVisible())
	{
		// In scroll bar area.
		ReleaseCapture();
		m_wndVertBar.SendMessage(WM_USER_ACLISTWND_MESSAGE_SETCAPTURE);

		if (IDC_ARROW != m_pCursorSourceID)
		{
			SetCursor(LoadCursor(NULL, IDC_ARROW));
			m_pCursorSourceID = IDC_ARROW;
		}
	}
	else
	{
		if (IDC_ARROW != m_pCursorSourceID)
		{
			SetCursor(LoadCursor(NULL, IDC_ARROW));
			m_pCursorSourceID = IDC_ARROW;
		}
	}

	CWnd::OnMouseMove(nFlags, point);
}

/*********************************************************************/
void CACListWnd::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) 
{
	if (GetSafeHwnd())
	{
		// Vers. 1.2 / fix 1.7 (vista border/m_SM_CYBORDER)
		long lMinY1 = GetSystemMetrics(SM_CYHSCROLL) * 2 + GetSystemMetrics(SM_CYSIZEFRAME) + GetSystemMetrics(SM_CXHTHUMB);
		long lMinY2 = m_lCount * m_lItemHeight + m_SM_CYBORDER * 2;

		if ((m_lVisibleItems > m_lCount - 1) && (lMinY2 < lMinY1))
		{
			lpMMI->ptMinTrackSize.y = lMinY2;
		}
		else
		{
			lpMMI->ptMinTrackSize.y = lMinY1;
		}
		
		lpMMI->ptMinTrackSize.x = GetSystemMetrics(SM_CXHSCROLL) * 4;

		// Vers. 1.2
		if (NULL != m_pEditParent) 
		{
			RECT rc;
			m_pEditParent->GetWindowRect(&rc);
			lpMMI->ptMinTrackSize.x = rc.right - rc.left;
		}
	}
	else
	{
		CWnd::OnGetMinMaxInfo(lpMMI);
	}
}

/*********************************************************************/
static bool SortListCompareStringAscend(const CACListDisplayNode* left, const CACListDisplayNode* right)
{
    int nCompareData = _tcsicmp(left->first, right->first);
    if (nCompareData < 0)
    {
        return true;
    }
    else if (0 == nCompareData)
    {
        if (left->second < right->second)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}

/*********************************************************************/
static bool SortListCompareStringDecend(const CACListDisplayNode* left, const CACListDisplayNode* right)
{
    int nCompareData = _tcsicmp(left->first, right->first);
    if (nCompareData > 0)
    {
        return true;
    }
    else if (0 == nCompareData)
    {
        if (left->second > right->second)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}

/*********************************************************************/
void CACListWnd::SortList(CACListDisplayList& aList)
{
    std::sort(aList->begin(), aList->end(), m_bSortDisplayListAscend ? SortListCompareStringAscend : SortListCompareStringDecend);
}

/*********************************************************************/
LRESULT CACListWnd::OnSetCapture(WPARAM wParam, LPARAM lParam)
{
	SetCapture();
	return S_OK;
}

/*********************************************************************/
LRESULT CACListWnd::OnReleaseCapture(WPARAM wParam, LPARAM lParam)
{
	ReleaseCapture();
	return S_OK;
}

/*********************************************************************/
void CACListWnd::ColorSpaceRGB2HSB(BYTE yRed, BYTE yGreen, BYTE yBlue, float& fHue, float& fSaturation, float& fBrightness)
{
    // Normalize red, green and blue.
    float fRed   = (float)yRed / 255.0f;
    float fGreen = (float)yGreen / 255.0f;
    float fBlue  = (float)yBlue / 255.0f;

    // Get the maximum and minimum component.
    float fMax = max(fRed, max(fGreen, fBlue));
    float fMin = min(fRed, min(fGreen, fBlue));

    // Get hue.
    if ((fabs(fMax - fRed) <= 0.00000001f) && ((fGreen - fBlue) >= 0.00000001f))
    {
        fHue = 60.0f * (fGreen - fBlue) / (fMax - fMin) + 0.0f;
    }
    else if ((fabs(fMax - fRed) <= 0.00000001f) && ((fGreen - fBlue) < 0.00000001f))
    {
        fHue = 60.0f * (fGreen - fBlue) / (fMax - fMin) + 360.0f;
    }
    else if (fabs(fMax - fGreen) <= 0.00000001f)
    {
        fHue = 60.0f * (fBlue - fRed) / (fMax - fMin) + 120.0f;
    }
    else if (fabs(fMax - fBlue) <= 0.00000001f)
    {
        fHue = 60.0f * (fRed - fGreen) / (fMax - fMin) + 240.0f;
    }
    else
    {
        fHue = 0.0f;
    }

    // Get saturation.
    if (fMax <= 0.00000001f)
    {
        fSaturation = 0.0f;
    }
    else
    {
        fSaturation = 1.0f - (fMin / fMax);
    }

    // Get brightness.
    fBrightness = fMax;
}

/*********************************************************************/
void CACListWnd::ColorSpaceHSB2RGB(float fHue, float fSaturation, float fBrightness, BYTE& yRed, BYTE& yGreen, BYTE& yBlue)
{
    if (fSaturation <= 0.00000001f)
    {
        yRed = yGreen = yBlue = 0;
        return;
    }

    // Validate the input.
    fHue        = max(0, min(fHue, 360.0f));
    fSaturation = max(0, min(fSaturation, 1.0f));
    fBrightness = max(0, min(fBrightness, 1.0f));

    // Init the color.
    float fRed   = 0.0f;
    float fGreen = 0.0f;
    float fBlue  = 0.0f;

    // Get parameters.
    // The color wheel consists of 6 sectors. Figure out which sector
    // you're in.
    int Hi = (int)(fHue / 60.0f) % 6;

    // get the fractional part of the sector
    float f = fHue / 60.0f - (float)Hi;

    // calculate values for the three axes of the color.
    float p = fBrightness * (1.0f - fSaturation);
    float q = fBrightness * (1.0f - fSaturation * f);
    float t = fBrightness * (1.0f - fSaturation * (1.0f - f));

    // Assign the fractional colors to r, g, and b based on the sector
    // the angle is in.
    switch (Hi)
    {
    case 0:
        fRed   = fBrightness;
        fGreen = t;
        fBlue  = p;
    	break;
    case 1:
        fRed   = q;
        fGreen = fBrightness;
        fBlue  = p;
        break;
    case 2:
        fRed   = p;
        fGreen = fBrightness;
        fBlue  = t;
        break;
    case 3:
        fRed   = p;
        fGreen = q;
        fBlue  = fBrightness;
        break;
    case 4:
        fRed   = t;
        fGreen = p;
        fBlue  = fBrightness;
        break;
    case 5:
        fRed   = fBrightness;
        fGreen = p;
        fBlue  = q;
        break;
    default:
        fRed   = 0.0f;
        fGreen = 0.0f;
        fBlue  = 0.0f;
        break;
    }

    yRed   = (BYTE)(fRed * 255.0f);
    yGreen = (BYTE)(fGreen * 255.0f);
    yBlue  = (BYTE)(fBlue * 255.0f);
}
