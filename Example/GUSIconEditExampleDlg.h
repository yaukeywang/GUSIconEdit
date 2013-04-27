/********************************************************************
	created : 2013/04/25
	filename: GUSIconEditExampleDlg.h
	author  : Yaukey(yaukeywang@gmail.com)
	purpose : The implementations of how to use GUSIconEdit.
              All rights reserved.
    Licence : The Code Project Open License (CPOL)
              http://www.codeproject.com/info/cpol10.aspx
*********************************************************************/

// GUSIconEditExampleDlg.h : 头文件
//

#pragma once

#include "GUSIconEdit.h"
#include <utility>

// CGUSIconEditExampleDlg 对话框
class CGUSIconEditExampleDlg : public CDialog
{
// 构造
public:
	CGUSIconEditExampleDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_GUSICONEDITEXAMPLE_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

protected:
    void InitGUSIconEditCtrl();
    void InitDataSetCtrl();
    void LoadDataSet();
    void ResetSearchResource();
    void InitCtrlMinSize();
    void AdjustCtrlLayout();

// 实现
protected:
	HICON m_hIcon;
    
    CGUSIconEdit m_cEdit1;
    CGUSIconEdit m_cEdit2;
    CGUSIconEdit m_cEdit3;

    CMenu m_cMenu1;
    CMenu m_cMenu2;
    CMenu m_cMenu3;

    std::pair<BOOL, BOOL> m_cSearchOpt1;
    std::pair<BOOL, BOOL> m_cSearchOpt2;
    std::pair<BOOL, BOOL> m_cSearchOpt3;

    int m_nMinRight;
    int m_nMinBottom;
    int m_nRightOffset;
    int m_nBottomOffset;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnTvnSelchangedTreectrl(NMHDR *pNMHDR, LRESULT *pResult);
    afx_msg void OnEnChangeEdit1();
    afx_msg void OnEnChangeEdit2();
    afx_msg void OnEnChangeEdit3();
    afx_msg LRESULT OnGUSIconEditLeftIconClicked(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnGUSIconEditRightIconClicked(WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnGUSIconEditSearchListClicked(WPARAM wParam, LPARAM lParam);
    afx_msg void OnSearchoptionCasesense1();
    afx_msg void OnSearchoptionMatchanywhere1();
    afx_msg void OnSearchoptionCasesense2();
    afx_msg void OnSearchoptionMatchanywhere2();
    afx_msg void OnSearchoptionCasesense3();
    afx_msg void OnSearchoptionMatchanywhere3();
    afx_msg void OnSize(UINT nType, int cx, int cy);
};
