/********************************************************************
	Created        : 2011/05/29
	Filename       : GUSIconEdit.h
    Original File  : IconEdit.h
    Original Author: Gautam Jain
    Modified By    : Yaukey(yaukeywang@gmail.com)
	
	Purpose: An extend CEdit with icon can be clicked as button.
	         This code used Gautam Jain's project named 
			 "iTunes like Find Edit Control" whose address is 
             http://www.codeproject.com/KB/edit/EditWithButton.aspx.

             I have rewrote part of the code to change some interfaces 
             to be simpler to use, to fix some bugs and enhance the
             functionality that it already has or not.

			 The edit's size is determined by the original width of the
             edit and the height of the skin the bitmap has provided.

             I give the new name "GUSIconEdit", "GUS" means "General Used for Search",
             because this edit is combined with a drop list control in order to
             give an general used searching control with other data set control such
             as CList, CListCtrl, CTreeCtrl and so on.

             If you have any problem about this modified code, just let me know by e-mail(yaukeywang@gmail.com).

             For more information about the original code, please visit:
             http://www.codeproject.com/KB/edit/EditWithButton.aspx.

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

#ifndef __GUS_ICON_EDIT_H__
#define __GUS_ICON_EDIT_H__

#include "ACListWnd.h"

//////////////////////////////////////////////////////////////////////////
// Message id.

// Begin message id of user clicked the icon of the edit. (Do not use this message.)
#define WM_USER_GUS_ICONEDIT_MESSAGE_BEGIN		WM_USER_ACLISTWND_MESSAGE_END + 1

// The message id that is sent to parent when the left icon is clicked.
// wParam - The control id.
// lParam - 0.
#define WM_USER_GUS_ICONEDIT_LEFT_ICON_CLICKED  WM_USER_GUS_ICONEDIT_MESSAGE_BEGIN + 1

// The message id that is sent to parent when the right icon is clicked.
// wParam - The control id.
// lParam - 0.
#define WM_USER_GUS_ICONEDIT_RIGHT_ICON_CLICKED WM_USER_GUS_ICONEDIT_MESSAGE_BEGIN + 2

// The message id that is sent to parent when the drop list is clicked.
// wParam - The control id.
// lParam - 0.
#define WM_USER_GUS_ICONEDIT_CLICK_SEARCH_LIST  WM_USER_GUS_ICONEDIT_MESSAGE_BEGIN + 3

//////////////////////////////////////////////////////////////////////////
// CGUSIcon Edit class.
class CGUSIconEdit : public CEdit
{
	DECLARE_DYNAMIC(CGUSIconEdit)

public:
	CGUSIconEdit();
	~CGUSIconEdit();

public:
	// Dynamic create a CGUSIconEdit.
	// If you design the edit in resource editor, you do not need to call this.
	BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);

	// Initialize the resource that CGUSIconEdit must have.
	// To let the icon edit work correctly, you must call this after the creating.
	BOOL InitializeResource(
		UINT   iEditTheme,			// The bitmap resource id of the theme picture.
		CRect& rcLIconEmpty,		// The left empty icon area on the bitmap.
		CRect& rcLIconFill,			// The left fill icon area on the bitmap.
		CRect& rcMIconEmpty,		// The middle empty icon area on the bitmap.
		CRect& rcMIconFill,			// The middle fill icon area on the bitmap.
		CRect& rcRIconEmpty,		// The right empty icon area on the bitmap.
		CRect& rcRIconFill,			// The right fill icon area on the bitmap.
		CRect& rcLBtn,				// The left button area within the left icon area (The offset in left icon).
		CRect& rcRBtn				// The right button area within the right icon area (The offset in right icon).
		);

	//////////////////////////////////////////////////////////////////////////
	// For edit.

	// Let the right button to show always or not.
	void ShowRightButtonAlways(BOOL bAlways);

	// Set the current text color of the edit, and return the old one.
	COLORREF SetTextColor(COLORREF clrTxt);

	// Set the current back ground color of the edit area of edit, and return the old one.
	COLORREF SetBkColor(COLORREF clrBk);

	// Set the empty tips of the edit.
	void SetEmptyTips(LPCTSTR strTips, COLORREF clrTips = RGB(128, 128, 128));

	// Set the text of the edit.
	void SetEditText(LPCTSTR strText);

	// Get the text of the edit.
	void GetEditText(CString& strText);

	// Get the length of text of the edit.
	int GetEditTextLength();

	// Clear the text of the edit.
	void ClearEditText();

	//////////////////////////////////////////////////////////////////////////
	// For search list.

    // Data source.

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

    // Find results.

    // Get total number of the results.
    int GetResultsCount() const;

	// Get the selected or the first string in the display list.
	LPCTSTR GetSelectedString() const;

	// Get the data of the selected or the first string in the display list.
	DWORD GetSelectedData() const;

    // Drop list properties.

	// To search with case sensitive or not.
	BOOL SetSearchCaseSense(BOOL bCaseSense);

	// To search always from the beginning or not. 
	BOOL SetSearchAnywhere(BOOL bAnywhere);

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
    // be automatically generated and be set.
    COLORREF SetSelItemBKColor(COLORREF clrSelItemBK, BOOL bAutoSetSelKeywordsColor = TRUE);

    // Set background color for selection item keywords.
    COLORREF SetSelItemKeywordsBKColor(COLORREF clrSelItemKeywordsBK);

    // Set text color for selection item.
    COLORREF SetSelItemTextColor(COLORREF clrSelItemText);

	// Set if the search list window should remember the new size after user resizing it.
	BOOL SetRememberWindowNewSize(BOOL bRemember);

    // Set if sort the display list of searching results, and ascend(TRUE) or decend(FALSE) if enabled sorting.
    BOOL SetSortDisplayedSearchResults(BOOL bSort, BOOL bAscend = TRUE);

    // Disable to popup the list window.
    BOOL DisabePopup(BOOL bDisable);

protected:
    void ResizeEditArea();
	void ResizeWindow();
	void UpdateContent(BOOL bEraseRect = TRUE);

public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
	virtual void PreSubclassWindow();

public:
	DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg void OnEnable(BOOL bEnable);
	afx_msg void OnEnChange();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg LRESULT OnSetFont(WPARAM wParam, LPARAM lParam);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg LRESULT OnClickSearchList(WPARAM wParam, LPARAM lParam);

protected:
	CBitmap m_bmpThemePic;			// The background theme picture.
	CSize   m_sizeThemePic;			// The size of the theme picture.

	CRect m_rcLeftIconEmpty;		// The left icon area on the theme picture of empty state.
	CRect m_rcLeftIconFill;			// The left icon area on the theme picture of fill state.
	CRect m_rcMiddleIconEmpty;		// The middle icon area on the theme picture of empty state.
	CRect m_rcMiddleIconFill;		// The middle icon area on the theme picture of fill state.
	CRect m_rcRightIconEmpty;		// The right icon area on the theme picture of empty state.
	CRect m_rcRightIconFill;		// The right icon area on the theme picture of fill state.

	CRect m_rcLeftBtnArea;			// The area of left button on edit.
	CRect m_rcRightBtnArea;			// The area of right button on edit.
    CRect m_rcRightBtnOriginArea;   // The origin area of right button on edit.

	CRect m_rcEditArea;				// The edit area of this edit.
	CRect m_rcEditSize;				// The current size of this edit.

	COLORREF m_clrTxt;				// The color of the text in edit.
	COLORREF m_clrEmptyTips;		// The color of the tips when edit is empty.
	COLORREF m_clrBk;				// The back ground of the edit.
	CBrush   m_cClrBrush;			// The text and back ground color brush.

	CString m_strEmptyTips;			// The string of the tips.
	BOOL    m_bEmpty;				// Indicates that the edit is currently empty.

	BOOL m_bIsRightBtnAlwaysShow;	// To show the right button always or not.

	CACListWnd m_cSearchListWnd;	// The search list.
	BOOL       m_bSearchCaseSense;	// To search with case sensitive or not.
	BOOL       m_bSearchAnywhere;	// To search always from the beginning or not.
};

#endif // __GUS_ICON_EDIT_H__
