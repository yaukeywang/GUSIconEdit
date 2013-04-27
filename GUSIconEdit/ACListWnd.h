/*********************************************************************
*
* CACListWnd.h 1.20
* Copyright (c) 2003-11 by Andreas Kapust
* All rights reserved.
* info@applications.de
*
* Modified by: Yaukey(yaukeywang@gmail.com)
*              - Change some interfaces at 2011.6.
*              - Height the keywords at 2011.6.
*              - Store the data of item assocated at 2011.6.
*              - Remove the timer, close when clicking the non-client area at 2012.10.
*              - Improved the search performance, add some color interfaces at 2012.12.
*
* If you have any problem about this modified code, just let me know by e-mail(yaukeywang@gmail.com).
*
* For more information about the original code, please visit:
* http://www.codeproject.com/Articles/2607/AutoComplete-without-IAutoComplete.
* 
* Licence: The Code Project Open License (CPOL)
*          http://www.codeproject.com/info/cpol10.aspx
*
*********************************************************************/

#if !defined(AFX_ACWND_H__5CED9BF8_C1CB_4A74_B022_ABA25680CC42__INCLUDED_)
#define AFX_ACWND_H__5CED9BF8_C1CB_4A74_B022_ABA25680CC42__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

//////////////////////////////////////////////////////////////////////////
// Message id.

// The begin message id of user.
#define WM_USER_ACLISTWND_MESSAGE_BEGIN				WM_USER + 3000

// The message id of user clicked the list.
#define WM_USER_ACLISTWND_MESSAGE_CLICK_LIST		WM_USER_ACLISTWND_MESSAGE_BEGIN + 1

// The message id of the list to set capture.
#define WM_USER_ACLISTWND_MESSAGE_SETCAPTURE		WM_USER_ACLISTWND_MESSAGE_BEGIN + 2

// The message id of the list to release capture.
#define WM_USER_ACLISTWND_MESSAGE_RELEASECAPTURES	WM_USER_ACLISTWND_MESSAGE_BEGIN + 3

// The end message id of user.
#define WM_USER_ACLISTWND_MESSAGE_END				WM_USER_ACLISTWND_MESSAGE_BEGIN + 4

/////////////////////////////////////////////////////////////////////////////
// CACListScrollBar
class CACListScrollBar : public CScrollBar
{
public:
	CACListScrollBar();
	~CACListScrollBar();

public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg LRESULT OnSetCapture(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnReleaseCapture(WPARAM wParam, LPARAM lParam);
};

/////////////////////////////////////////////////////////////////////////////
// CACListWnd

// Some class references.
class CACListSearchNode;
class CACListDisplayNode;
class CACListSearchList;
class CACListDisplayList;

// The main window of display list window.
class CACListWnd : public CWnd
{
// Construction and destruction.
public:
	CACListWnd();
	virtual ~CACListWnd();

public:
    //////////////////////////////////////////////////////////////////////////
    // Data source.

	// Create and initialize a list window.
	BOOL CreateAndInit(CWnd* pParentWnd);

	// Add a source string to search.
	void AddSearchString(LPCTSTR lpszString, DWORD dwData = UINT_MAX);

	// Remove the item whose index is nItem.
	void RemoveAt(int nItem);

	// Remove all the search string.
	void RemoveAll();

    // Get the total number of data source(The search strings).
    int GetSourcesCount() const;

	// Get the source text of the item whose index is nItem.
	BOOL GetSourceText(int nItem, CString& strText);

	// Get the source data of the item whose index is nItem.
	DWORD GetSourceData(int nItem);

    //////////////////////////////////////////////////////////////////////////
    // Find results.

	// Find a substring in search list, and put them all in the display list to popup.
	// lpszString   - The keywords to searching for.
	// bCaseSense   - Case sensitive or not.
	// bMatchAny    - Match the key words in anywhere of the source string or not.
	// nStartAfter  - The index in the search list After which the searching begins.
	// bDisplayOnly - Set true just pop-up to display the existing results in the display list.
	// return value - The number of finding results, -1 for other reasons(Error).
	int FindString(LPCTSTR lpszString, BOOL bCaseSense = FALSE, BOOL bMatchAny = TRUE, int nStartAfter = -1, BOOL bDisplayOnly = FALSE);

	// Find a string that completely matches the lpszString, and return the index, -1 no result.
	// nIndexStart - The index in the search list After which the searching begins.
	int FindStringExact(LPCTSTR lpszString, int nStartAfter = -1);

	// Find string in search list, then, popup the results and select the first one.
	int SelectString(LPCTSTR lpszString);

    // Select an item on the results list.
    BOOL SelectItem(int nItem);

    // Get total number of the results.
    int GetResultsCount() const;
	
	// Get the selected or the first string in the display list.
	LPCTSTR GetSelectedString() const;

	// Get the data of the selected or the first string in the display list.
	DWORD GetSelectedData() const;

	// Change the current select item and return the string of the new item.
	LPCTSTR GetNextString(int nChar);

    //////////////////////////////////////////////////////////////////////////
    // Drop list properties.

    // Set max items to display.
    BYTE SetMaxDisplayItem(BYTE yItem);

    // Set background color for normal items.
    COLORREF SetNormalItemsBKColor(COLORREF clrNormalItemsBK);

	// Set background color for even items.
	COLORREF SetEvenItemsBKColor(COLORREF clrEvenItemsBK);

	// Set background color for non-select items keywords. 
	COLORREF SetNonSelItemsKeywordsBKColor(COLORREF clrNonSelItemsKeywordsBK);

    // Set text color for normal items.
    COLORREF SetNormalItemsTextColor(COLORREF clrNormalItemsText);

    // Set background color for selection item.
    // If bAutoSetSelKeywordsColor is set TRUE, an color for SetSelItemKeywordsBKColor() will
    // be automatic generated and be set.
    COLORREF SetSelItemBKColor(COLORREF clrSelItemBK, BOOL bAutoSetSelKeywordsColor = TRUE);

    // Set background color for selection item keywords.
    COLORREF SetSelItemKeywordsBKColor(COLORREF clrSelItemKeywordsBK);

    // Set text color for selection item.
    COLORREF SetSelItemTextColor(COLORREF clrSelItemText);

	// Set if the window should remember the new size after user resizing it.
	BOOL SetRememberWindowNewSize(BOOL bRemember);

    // Set if sort the display list of searching results, and ascend(TRUE) or decend(FALSE) if enabled sorting.
    BOOL SetSortDisplayedSearchResults(BOOL bSort, BOOL bAscend);

    // Disable to popup the list window.
    BOOL DisabePopup(BOOL bDisable = TRUE);

	// Process the mouse wheel event.
	BOOL DoMouseWheel(UINT nFlags, short zDelta, CPoint pt);

public:
    // The transformation of color space between RGB and HSB.
    // yRed, yGreen, yBlue - [0, 255]
    // fHue - [0, 1]
    // fSaturation, fBrightness - [0, 1]
    // http://www.codeproject.com/Articles/19045/Manipulating-colors-in-NET-Part-1#intro
    static void ColorSpaceRGB2HSB(BYTE yRed, BYTE yGreen, BYTE yBlue, float& fHue, float& fSaturation, float& fBrightness);
    static void ColorSpaceHSB2RGB(float fHue, float fSaturation, float fBrightness, BYTE& yRed, BYTE& yGreen, BYTE& yBlue);

protected:
    BOOL EnsureVisible(int nItem, BOOL bWait);
	void DrawItem(CDC* pDC, long lItem, long lWidth);
	void InvalidateAndScroll();
	void AdjustListPosition(CRect& rcWnd);
	void SetScroller();
	void SetProp();
	int HitTest(CPoint point);
	long GetScrollBarWidth();
	void OnChangeWindowSize();
    void SortList(CACListDisplayList& aList);

protected:
	CACListScrollBar m_wndVertBar;		        // Vertical scroll bar.
	CACListScrollBar m_wndHoriBar;		        // Horizontal scroll bar.

    int m_SM_CXBORDER;                          // Size of border on width of the list window.
    int m_SM_CYBORDER;                          // Size of border on height of the list window.

	CACListSearchList*  m_pSearchList;          // The data source.
	CACListDisplayList* m_pDisplayList;         // The current displayed list.
    CACListSearchNode*  m_pSearchNodeAdder;     // The search data node add helper.
    CACListDisplayNode* m_pDisplayNodeAdder;    // The display data node add helper.

	CString m_strDisplay;                       // Displayed text on item text.
	TCHAR   m_cPrefixChar;                      // The prefix char.
	BOOL    m_bListIsDisabled;                  // Disable to pop-up list or not.
	BYTE    m_yMaxDisplayItem;                  // How many items to display in the list atomically.
    BOOL    m_bSortDisplayList;                 // Do sort display list or not.
    BOOL    m_bSortDisplayListAscend;           // Do sort display list ascend(TRUE) or decend(FALSE).
    int     m_nKeywordsLen;                     // The length of the keywords.

	CRect  m_rcLastSize;				        // The last size of window.
	CEdit* m_pEditParent;				        // The parent edit.

	LPCTSTR m_pCursorSourceID;			        // The source id of the current cursor.
	BOOL    m_bResizing;				        // Tag we are resizing or not.
	int     m_nMouseDiffX;				        // The offset between cursor point and window width.
	int     m_nMouseDiffY;				        // The offset between cursor point and window height.
	BOOL    m_bRememberNewSize;			        // Remember the window's new size or not after user resizing the window.

	long m_lTopIndex;                           // The index of the first displayed item.
	long m_lCount;                              // How many items in searched results.
	long m_lItemHeight;                         // The height of each item.
	long m_lVisibleItems;                       // How many items currently visible.
	long m_lSelItem;                            // The index of the currently selected item.

    COLORREF m_clrNormalItemsBK;                // The color of background on normal items.
	COLORREF m_clrEvenItemsBK;                  // The color of background on even items.
	COLORREF m_clrNonSelItemsKeywordsBK;        // The color of background of keywords on non-select items.
    COLORREF m_clrNormalItemsText;              // The color of text on normal items.
   
    COLORREF m_clrSelItemBK;                    // The color of background on selected items.
    COLORREF m_clrSelItemKeywordsBK;            // The color of background of keywords on selected items.
    COLORREF m_clrSelItemText;                  // The color of text on selected items.


	// Generierte Nachrichtenzuordnungsfunktionen
protected:
	DECLARE_MESSAGE_MAP()

	//{{AFX_VIRTUAL(CACListWnd)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CACListWnd)
	afx_msg void OnPaint();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnActivateApp(BOOL bActive, DWORD dwThreadID);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnNcLButtonDown(UINT nHitTest, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);

 #if _MFC_VER < 0x0800
	afx_msg UINT OnNcHitTest(CPoint point);
#else
	afx_msg LRESULT OnNcHitTest(CPoint point);
#endif

	afx_msg LRESULT OnSetCapture(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnReleaseCapture(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ ft unmittelbar vor der vorhergehenden Zeile zuszliche Deklarationen ein.

#endif // AFX_ACWND_H__5CED9BF8_C1CB_4A74_B022_ABA25680CC42__INCLUDED_
