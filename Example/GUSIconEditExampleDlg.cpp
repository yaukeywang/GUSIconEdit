/********************************************************************
	created : 2013/04/25
	filename: GUSIconEditExampleDlg.cpp
	author  : Yaukey(yaukeywang@gmail.com)
	purpose : The implementations of how to use GUSIconEdit.
              All rights reserved.
    Licence : The Code Project Open License (CPOL)
              http://www.codeproject.com/info/cpol10.aspx
*********************************************************************/

// GUSIconEditExampleDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "GUSIconEditExample.h"
#include "GUSIconEditExampleDlg.h"
#include "GUSIconEditDataSourceLoader.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CGUSIconEditExampleDlg 对话框




CGUSIconEditExampleDlg::CGUSIconEditExampleDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGUSIconEditExampleDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

    m_cSearchOpt1.first  = FALSE;
    m_cSearchOpt1.second = TRUE;

    m_cSearchOpt2.first  = FALSE;
    m_cSearchOpt2.second = TRUE;

    m_cSearchOpt3.first  = FALSE;
    m_cSearchOpt3.second = TRUE;

    m_nMinRight     = 0;
    m_nMinBottom    = 0;
    m_nRightOffset  = 0;
    m_nBottomOffset = 0;
}

void CGUSIconEditExampleDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

    DDX_Control(pDX, IDC_EDIT1, m_cEdit1);
    DDX_Control(pDX, IDC_EDIT2, m_cEdit2);
}

void CGUSIconEditExampleDlg::InitGUSIconEditCtrl()
{
    // Init the first edit.
    // We just use default parameters.
    // You can find the skin rect from "GUSIconEditSkin.bmp" and "GUSIconEditSkin.offset.txt".
    m_cEdit1.InitializeResource(
        IDB_BITMAP_GUS_ICON_EDIT_SKIN,
        CRect(3, 69, 27, 90),
        CRect(3, 69, 27, 90),
        CRect(26, 47, 27, 68),
        CRect(26, 47, 27, 68),
        CRect(48, 47, 67, 68),
        CRect(28, 69, 47, 90),
        CRect(5, 5, 22, 22),
        CRect(1, 4, 14, 17)
        );

    m_cEdit1.SetEmptyTips(_T("Search items in ListBox..."));
    m_cMenu1.LoadMenu(IDR_MENU_GUS_ICON_EDIT1);

    // Init the second edit.
    // We try to change some parameters.
    // You can find the skin rect from "GUSIconEditSkin.bmp" and "GUSIconEditSkin.offset.txt".
    m_cEdit2.InitializeResource(
        IDB_BITMAP_GUS_ICON_EDIT_SKIN,
        CRect(3, 69, 27, 90),
        CRect(3, 69, 27, 90),
        CRect(26, 47, 27, 68),
        CRect(26, 47, 27, 68),
        CRect(48, 47, 67, 68),
        CRect(28, 69, 47, 90),
        CRect(5, 5, 22, 22),
        CRect(1, 4, 14, 17)
        );

    m_cEdit2.SetEmptyTips(_T("Search items in ListCtrl..."));
    m_cEdit2.SetMaxDisplayItem(20);
    m_cEdit2.SetNormalItemsBKColor(RGB(201, 241, 238));
    m_cEdit2.SetEvenItemsBKColor(RGB(201, 187, 234));
    m_cEdit2.SetNonSelItemsKeywordsBKColor(RGB(248, 134, 151));
    m_cEdit2.SetNormalItemsTextColor(RGB(104, 36, 94));
    m_cEdit2.SetSelItemBKColor(RGB(221, 215, 89), TRUE);
    m_cEdit2.SetSelItemTextColor(RGB(203, 7, 75));
    m_cEdit2.SetRememberWindowNewSize(TRUE);
    m_cEdit2.SetSortDisplayedSearchResults(TRUE, TRUE);
    m_cMenu2.LoadMenu(IDR_MENU_GUS_ICON_EDIT2);

    // Init the third edit.
    // Dynamically create.
    // You can find the skin rect from "GUSIconEditSkin.bmp" and "GUSIconEditSkin.offset.txt".
    CRect rcEdit3;
    GetDlgItem(IDC_LABEL_ALIGMENT)->GetWindowRect(rcEdit3);
    ScreenToClient(rcEdit3);

    m_cEdit3.Create(
        WS_CHILD | WS_VISIBLE | ES_MULTILINE | WS_TABSTOP | ES_AUTOHSCROLL | WS_BORDER,
        rcEdit3, this, IDC_EDIT3
        );

    m_cEdit3.InitializeResource(
        IDB_BITMAP_GUS_ICON_EDIT_SKIN,
        CRect(3, 69, 27, 90),
        CRect(3, 69, 27, 90),
        CRect(26, 47, 27, 68),
        CRect(26, 47, 27, 68),
        CRect(48, 47, 67, 68),
        CRect(28, 47, 47, 68),
        CRect(5, 5, 22, 22),
        CRect(1, 4, 14, 17)
        );

    m_cEdit3.SetEmptyTips(_T("Search items in TreeCtrl..."));
    m_cEdit3.SetEvenItemsBKColor(::GetSysColor(COLOR_WINDOW));
    m_cEdit3.SetSelItemBKColor(RGB(169, 87, 230), FALSE);
    m_cEdit3.SetSelItemKeywordsBKColor(RGB(3, 231, 180));
    m_cEdit3.SetSortDisplayedSearchResults(TRUE, FALSE);
    m_cMenu3.LoadMenu(IDR_MENU_GUS_ICON_EDIT3);
}

void CGUSIconEditExampleDlg::InitDataSetCtrl()
{
    // Init the list box.

    // Init the list ctrl.
    CListCtrl* pListCtrl = (CListCtrl*)GetDlgItem(IDC_LISTCTRL);
    pListCtrl->SetExtendedStyle(pListCtrl->GetExtendedStyle() | LVS_EX_FULLROWSELECT);
    pListCtrl->InsertColumn(0, _T("ID"), LVCFMT_LEFT, 50);
    pListCtrl->InsertColumn(1, _T("Content"), LVCFMT_LEFT, 127);

    // Init the tree ctrl.
}

void CGUSIconEditExampleDlg::LoadDataSet()
{
    // Load data source for ListBox.
    CListBoxDataLoader cListBoxDataLoader(_T("ListBoxData.txt"), CYKTxtArchiveSerializer::OM_READONLY);
    cListBoxDataLoader.m_pListBox = (CListBox*)GetDlgItem(IDC_LISTBOX);
    if (!cListBoxDataLoader.Go())
    {
        MessageBox(_T("Load ListBox data failed!"), _T("Load Data"));
    }

    // Load data source for ListCtrl.
    CListCtrlDataLoader cListCtrlDataLoader(_T("ListCtrlData.txt"), CYKTxtArchiveSerializer::OM_READONLY);
    cListCtrlDataLoader.m_pListCtrl = (CListCtrl*)GetDlgItem(IDC_LISTCTRL);
    if (!cListCtrlDataLoader.Go())
    {
        MessageBox(_T("Load ListCtrl data failed!"), _T("Load Data"));
    }

    // Load data source for TreeCtrl.
    CTreeCtrlDataLoader cTreeCtrlDataLoader(_T("TreeCtrlData.txt"), CYKTxtArchiveSerializer::OM_READONLY);
    cTreeCtrlDataLoader.m_pTreeCtrl = (CTreeCtrl*)GetDlgItem(IDC_TREECTRL);
    if (!cTreeCtrlDataLoader.Go())
    {
        MessageBox(_T("Load TreeCtrl data failed!"), _T("Load Data"));
    }
}

void CGUSIconEditExampleDlg::ResetSearchResource()
{
    // Add data source from ListBox.
    m_cEdit1.RemoveAll();
    CListBox* pListBox = (CListBox*)GetDlgItem(IDC_LISTBOX);
    CString   strTxt;

    int nDataCount = pListBox->GetCount();
    for (int i = 0; i < nDataCount; i++)
    {
        pListBox->GetText(i, strTxt);
        m_cEdit1.AddSearchString(strTxt, i);
    }

    //  Add data source from ListCtrl.
    m_cEdit2.RemoveAll();
    CListCtrl* pListCtrl = (CListCtrl*)GetDlgItem(IDC_LISTCTRL);

    nDataCount = pListCtrl->GetItemCount();
    for (int i = 0; i < nDataCount; i++)
    {
        strTxt  = pListCtrl->GetItemText(i, 1);
        strTxt += _T("-");
        strTxt += pListCtrl->GetItemText(i, 0);

        m_cEdit2.AddSearchString(strTxt, i);
    }

    // Add data source from TreeCtrl.
    m_cEdit3.RemoveAll();
    CTreeCtrl* pTreeCtrl = (CTreeCtrl*)GetDlgItem(IDC_TREECTRL);

    // 3 layers at most including root item.
    HTREEITEM hCurItem   = pTreeCtrl->GetRootItem();
    HTREEITEM hChildItem = NULL;
    while (NULL != hCurItem)
    {
        // Get children.
        hChildItem = pTreeCtrl->GetChildItem(hCurItem);
        while (NULL != hChildItem)
        {
            strTxt = pTreeCtrl->GetItemText(hChildItem);
            m_cEdit3.AddSearchString(strTxt, (DWORD)hChildItem);

            hChildItem = pTreeCtrl->GetNextSiblingItem(hChildItem);
        }

        // Next.
        hCurItem = pTreeCtrl->GetNextSiblingItem(hCurItem);
    }
}

void CGUSIconEditExampleDlg::InitCtrlMinSize()
{
    // Remember some size in order to calculate new size for some ctrl.
    CRect rcWnd;
    GetWindowRect(rcWnd);
    ScreenToClient(rcWnd);

    CRect rcRightOffset;
    m_cEdit3.GetWindowRect(rcRightOffset);
    ScreenToClient(rcRightOffset);

    m_nMinRight    = rcRightOffset.right;
    m_nRightOffset = rcWnd.right - rcRightOffset.right;

    CRect rcBottomOffset;
    CTreeCtrl* pTreeCtrl = (CTreeCtrl*)GetDlgItem(IDC_TREECTRL);
    pTreeCtrl->GetWindowRect(rcBottomOffset);
    ScreenToClient(rcBottomOffset);

    m_nMinBottom    = rcBottomOffset.bottom;
    m_nBottomOffset = rcWnd.bottom - rcBottomOffset.bottom;
}

void CGUSIconEditExampleDlg::AdjustCtrlLayout()
{
    // Calculate new rectangle and offset to right border and bottom border of dialog.
    if (NULL == m_cEdit3.GetSafeHwnd())
    {
        return;
    }

    // Get window size;
    CRect rcWnd;
    GetWindowRect(rcWnd);
    ScreenToClient(rcWnd);

    // Calculate new size.
    CRect rcNewSize;
    CRect rcOldSize;

    // Edit3.
    m_cEdit3.GetWindowRect(rcOldSize);
    ScreenToClient(rcOldSize);

    rcNewSize.SetRect(rcOldSize.left, rcOldSize.top, rcWnd.right - m_nRightOffset, rcOldSize.bottom);
    if (rcNewSize.right < m_nMinRight)
    {
        rcNewSize.right = m_nMinRight;
    }

    m_cEdit3.SetWindowPos(NULL, rcNewSize.left, rcNewSize.top, rcNewSize.Width(), rcNewSize.Height(), SWP_NOACTIVATE | SWP_NOZORDER);

    // Tree ctrl.
    CTreeCtrl* pTreeCtrl = (CTreeCtrl*)GetDlgItem(IDC_TREECTRL);
    pTreeCtrl->GetWindowRect(rcOldSize);
    ScreenToClient(rcOldSize);

    rcNewSize.SetRect(rcOldSize.left, rcOldSize.top, rcWnd.right - m_nRightOffset, rcWnd.bottom - m_nBottomOffset);
    if (rcNewSize.right < m_nMinRight)
    {
        rcNewSize.right = m_nMinRight;
    }

    if (rcNewSize.bottom < m_nMinBottom)
    {
        rcNewSize.bottom = m_nMinBottom;
    }

    pTreeCtrl->SetWindowPos(NULL, rcNewSize.left, rcNewSize.top, rcNewSize.Width(), rcNewSize.Height(), SWP_NOACTIVATE | SWP_NOZORDER);

    // List ctrl.
    CListCtrl* pListCtrl = (CListCtrl*)GetDlgItem(IDC_LISTCTRL);
    pListCtrl->GetWindowRect(rcOldSize);
    ScreenToClient(rcOldSize);

    rcNewSize.SetRect(rcOldSize.left, rcOldSize.top, rcOldSize.right, rcWnd.bottom - m_nBottomOffset);
    if (rcNewSize.bottom < m_nMinBottom)
    {
        rcNewSize.bottom = m_nMinBottom;
    }

    pListCtrl->SetWindowPos(NULL, rcNewSize.left, rcNewSize.top, rcNewSize.Width(), rcNewSize.Height(), SWP_NOACTIVATE | SWP_NOZORDER);

    // List box.
    CListBox* pListBox = (CListBox*)GetDlgItem(IDC_LISTBOX);
    pListBox->GetWindowRect(rcOldSize);
    ScreenToClient(rcOldSize);
    
    rcNewSize.SetRect(rcOldSize.left, rcOldSize.top, rcOldSize.right, rcWnd.bottom - m_nBottomOffset);
    if (rcNewSize.bottom < m_nMinBottom)
    {
        rcNewSize.bottom = m_nMinBottom;
    }

    pListBox->SetWindowPos(NULL, rcNewSize.left, rcNewSize.top, rcNewSize.Width(), rcNewSize.Height(), SWP_NOACTIVATE | SWP_NOZORDER);
}

BEGIN_MESSAGE_MAP(CGUSIconEditExampleDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
    ON_NOTIFY(TVN_SELCHANGED, IDC_TREECTRL, &CGUSIconEditExampleDlg::OnTvnSelchangedTreectrl)
    ON_EN_CHANGE(IDC_EDIT1, &CGUSIconEditExampleDlg::OnEnChangeEdit1)
    ON_EN_CHANGE(IDC_EDIT2, &CGUSIconEditExampleDlg::OnEnChangeEdit2)
    ON_EN_CHANGE(IDC_EDIT3, &CGUSIconEditExampleDlg::OnEnChangeEdit3)
    ON_MESSAGE(WM_USER_GUS_ICONEDIT_LEFT_ICON_CLICKED, &CGUSIconEditExampleDlg::OnGUSIconEditLeftIconClicked)
    ON_MESSAGE(WM_USER_GUS_ICONEDIT_RIGHT_ICON_CLICKED, &CGUSIconEditExampleDlg::OnGUSIconEditRightIconClicked)
    ON_MESSAGE(WM_USER_GUS_ICONEDIT_CLICK_SEARCH_LIST, &CGUSIconEditExampleDlg::OnGUSIconEditSearchListClicked)
    ON_COMMAND(ID_SEARCHOPTION_CASESENSE1, &CGUSIconEditExampleDlg::OnSearchoptionCasesense1)
    ON_COMMAND(ID_SEARCHOPTION_MATCHANYWHERE1, &CGUSIconEditExampleDlg::OnSearchoptionMatchanywhere1)
    ON_COMMAND(ID_SEARCHOPTION_CASESENSE2, &CGUSIconEditExampleDlg::OnSearchoptionCasesense2)
    ON_COMMAND(ID_SEARCHOPTION_MATCHANYWHERE2, &CGUSIconEditExampleDlg::OnSearchoptionMatchanywhere2)
    ON_COMMAND(ID_SEARCHOPTION_CASESENSE3, &CGUSIconEditExampleDlg::OnSearchoptionCasesense3)
    ON_COMMAND(ID_SEARCHOPTION_MATCHANYWHERE3, &CGUSIconEditExampleDlg::OnSearchoptionMatchanywhere3)
    ON_WM_SIZE()
END_MESSAGE_MAP()


// CGUSIconEditExampleDlg 消息处理程序

BOOL CGUSIconEditExampleDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
    InitGUSIconEditCtrl();
    InitDataSetCtrl();
    LoadDataSet();
    ResetSearchResource();
    InitCtrlMinSize();

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CGUSIconEditExampleDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CGUSIconEditExampleDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CGUSIconEditExampleDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CGUSIconEditExampleDlg::OnTvnSelchangedTreectrl(NMHDR *pNMHDR, LRESULT *pResult)
{
    LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
    // TODO: 在此添加控件通知处理程序代码
    *pResult = 0;
}

void CGUSIconEditExampleDlg::OnEnChangeEdit1()
{
    // TODO:  如果该控件是 RICHEDIT 控件，它将不
    // 发送此通知，除非重写 CDialog::OnInitDialog()
    // 函数并调用 CRichEditCtrl().SetEventMask()，
    // 同时将 ENM_CHANGE 标志“或”运算到掩码中。

    // TODO:  在此添加控件通知处理程序代码
    if (m_cEdit1.GetResultsCount() > 0)
    {
        m_cEdit1.SetTextColor(RGB(0, 0, 0));
    }
    else
    {
        m_cEdit1.SetTextColor(RGB(255, 0, 0));
    }
}

void CGUSIconEditExampleDlg::OnEnChangeEdit2()
{
    // TODO:  如果该控件是 RICHEDIT 控件，它将不
    // 发送此通知，除非重写 CDialog::OnInitDialog()
    // 函数并调用 CRichEditCtrl().SetEventMask()，
    // 同时将 ENM_CHANGE 标志“或”运算到掩码中。

    // TODO:  在此添加控件通知处理程序代码
    if (m_cEdit2.GetResultsCount() > 0)
    {
        m_cEdit2.SetTextColor(RGB(0, 0, 0));
    }
    else
    {
        m_cEdit2.SetTextColor(RGB(255, 0, 0));
    }
}

void CGUSIconEditExampleDlg::OnEnChangeEdit3()
{
    if (m_cEdit3.GetResultsCount() > 0)
    {
        m_cEdit3.SetTextColor(RGB(0, 0, 0));
    }
    else
    {
        m_cEdit3.SetTextColor(RGB(255, 0, 0));
    }
}

LRESULT CGUSIconEditExampleDlg::OnGUSIconEditLeftIconClicked(WPARAM wParam, LPARAM lParam)
{
    CRect rcEdit;

    int nCtrlId = (int)wParam;
    if (IDC_EDIT1 == nCtrlId)
    {
        m_cEdit1.GetWindowRect(&rcEdit);

        CMenu* pMenu = m_cMenu1.GetSubMenu(0);
        pMenu->CheckMenuItem(ID_SEARCHOPTION_CASESENSE1, m_cSearchOpt1.first ? MF_CHECKED : MF_UNCHECKED);
        pMenu->CheckMenuItem(ID_SEARCHOPTION_MATCHANYWHERE1, m_cSearchOpt1.second ? MF_CHECKED : MF_UNCHECKED);
        pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON, rcEdit.left, rcEdit.bottom, this);
    }
    else if (IDC_EDIT2 == nCtrlId)
    {
        m_cEdit2.GetWindowRect(&rcEdit);

        CMenu* pMenu = m_cMenu2.GetSubMenu(0);
        pMenu->CheckMenuItem(ID_SEARCHOPTION_CASESENSE2, m_cSearchOpt2.first ? MF_CHECKED : MF_UNCHECKED);
        pMenu->CheckMenuItem(ID_SEARCHOPTION_MATCHANYWHERE2, m_cSearchOpt2.second ? MF_CHECKED : MF_UNCHECKED);
        pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON, rcEdit.left, rcEdit.bottom, this);
    }
    else if (IDC_EDIT3 == nCtrlId)
    {
        m_cEdit3.GetWindowRect(&rcEdit);

        CMenu* pMenu = m_cMenu3.GetSubMenu(0);
        pMenu->CheckMenuItem(ID_SEARCHOPTION_CASESENSE3, m_cSearchOpt3.first ? MF_CHECKED : MF_UNCHECKED);
        pMenu->CheckMenuItem(ID_SEARCHOPTION_MATCHANYWHERE3, m_cSearchOpt3.second ? MF_CHECKED : MF_UNCHECKED);
        pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON, rcEdit.left, rcEdit.bottom, this);
    }

    return 1;
}

LRESULT CGUSIconEditExampleDlg::OnGUSIconEditRightIconClicked(WPARAM wParam, LPARAM lParam)
{
    int nCtrlId = (int)wParam;
    if (IDC_EDIT1 == nCtrlId)
    {
        if (m_cEdit1.GetEditTextLength() > 0)
        {
            m_cEdit1.ClearEditText();
        }
    }
    else if (IDC_EDIT2 == nCtrlId)
    {
        if (m_cEdit2.GetEditTextLength() > 0)
        {
            m_cEdit2.ClearEditText();
        }
    }
    else if (IDC_EDIT3 == nCtrlId)
    {
        if (m_cEdit3.GetEditTextLength() > 0)
        {
            m_cEdit3.ClearEditText();
        }
    }

    return 1;
}

LRESULT CGUSIconEditExampleDlg::OnGUSIconEditSearchListClicked(WPARAM wParam, LPARAM lParam)
{
    int nCtrlId = (int)wParam;
    if (IDC_EDIT1 == nCtrlId)
    {
        int nSelIdx = (int)m_cEdit1.GetSelectedData();
        if (nSelIdx >= 0)
        {
            CListBox* pListBox = (CListBox*)GetDlgItem(IDC_LISTBOX);
            pListBox->SetCurSel(nSelIdx);
            pListBox->SetFocus();
        }
    }
    else if (IDC_EDIT2 == nCtrlId)
    {
        int nSelIdx = (int)m_cEdit2.GetSelectedData();
        if (nSelIdx >= 0)
        {
            CListCtrl* pListCtrl = (CListCtrl*)GetDlgItem(IDC_LISTCTRL);
            
            int nOldIdx = pListCtrl->GetNextItem(-1, LVNI_SELECTED);
            if (nOldIdx >= 0)
            {
                pListCtrl->SetItemState(nOldIdx, LVIS_SELECTED, ~LVIS_SELECTED);
            }

            pListCtrl->SetItemState(nSelIdx, LVIS_SELECTED, LVIS_SELECTED);
            pListCtrl->EnsureVisible(nSelIdx, FALSE);
            pListCtrl->SetFocus();
        }
    }
    else if (IDC_EDIT3 == nCtrlId)
    {
        HTREEITEM hSelItem = (HTREEITEM)m_cEdit3.GetSelectedData();
        if (NULL != hSelItem)
        {
            CTreeCtrl* pTreeCtrl = (CTreeCtrl*)GetDlgItem(IDC_TREECTRL);
            pTreeCtrl->SelectItem(hSelItem);
            pTreeCtrl->EnsureVisible(hSelItem);
            pTreeCtrl->SetFocus();
        }
    }

    return 1;
}

void CGUSIconEditExampleDlg::OnSearchoptionCasesense1()
{
    // TODO: 在此添加命令处理程序代码
    m_cSearchOpt1.first = !m_cSearchOpt1.first;
    m_cEdit1.SetSearchCaseSense(m_cSearchOpt1.first);
}

void CGUSIconEditExampleDlg::OnSearchoptionMatchanywhere1()
{
    // TODO: 在此添加命令处理程序代码
    m_cSearchOpt1.second = !m_cSearchOpt1.second;
    m_cEdit1.SetSearchAnywhere(m_cSearchOpt1.second);
}

void CGUSIconEditExampleDlg::OnSearchoptionCasesense2()
{
    // TODO: 在此添加命令处理程序代码
    m_cSearchOpt2.first = !m_cSearchOpt2.first;
    m_cEdit2.SetSearchCaseSense(m_cSearchOpt2.first);
}

void CGUSIconEditExampleDlg::OnSearchoptionMatchanywhere2()
{
    // TODO: 在此添加命令处理程序代码
    m_cSearchOpt2.second = !m_cSearchOpt2.second;
    m_cEdit2.SetSearchAnywhere(m_cSearchOpt2.second);
}

void CGUSIconEditExampleDlg::OnSearchoptionCasesense3()
{
    // TODO: 在此添加命令处理程序代码
    m_cSearchOpt3.first = !m_cSearchOpt3.first;
    m_cEdit3.SetSearchCaseSense(m_cSearchOpt3.first);
}

void CGUSIconEditExampleDlg::OnSearchoptionMatchanywhere3()
{
    // TODO: 在此添加命令处理程序代码
    m_cSearchOpt3.second = !m_cSearchOpt3.second;
    m_cEdit3.SetSearchAnywhere(m_cSearchOpt3.second);
}

void CGUSIconEditExampleDlg::OnSize(UINT nType, int cx, int cy)
{
    CDialog::OnSize(nType, cx, cy);

    AdjustCtrlLayout();
}
