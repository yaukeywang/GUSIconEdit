/********************************************************************
	created : 2013/04/25
	filename: GUSIconEditDataSourceLoader.cpp
	author  : Yaukey(yaukeywang@gmail.com)
	purpose : Classes for loading data source to data set control.
              All rights reserved.
    Licence : The Code Project Open License (CPOL)
              http://www.codeproject.com/info/cpol10.aspx
*********************************************************************/

#include "stdafx.h"
#include "GUSIconEditDataSourceLoader.h"

//////////////////////////////////////////////////////////////////////////
// Data loader for CListBox.
unsigned long CListBoxDataLoader::Serialize(CYKTxtArchive& ar)
{
    if (!ar.IsLoading())
    {
        ASSERT(0);
        return 0;
    }

    if (NULL == m_pListBox)
    {
        ASSERT(0);
        return 0;
    }

    YKString strContent;
    ar.ReadEntireText(strContent);
    if (strContent.empty())
    {
        return 1;
    }

    strContent += _YKT('\n');

    YKString strItem;
    while (!strContent.empty())
    {
        strContent >> strItem;
        m_pListBox->InsertString(m_pListBox->GetCount(), strItem.c_str());
        strItem.clear();
    }

    return 1;
}

//////////////////////////////////////////////////////////////////////////
// Data loader for CListCtrl.
unsigned long CListCtrlDataLoader::Serialize(CYKTxtArchive& ar)
{
    if (!ar.IsLoading())
    {
        ASSERT(0);
        return 0;
    }

    if (NULL == m_pListCtrl)
    {
        ASSERT(0);
        return 0;
    }

    YKString strContent;
    ar.ReadEntireText(strContent);
    if (strContent.empty())
    {
        return 1;
    }

    strContent += _YKT('\n');

    YKString strItem;
    int      nIndex = 0;
    while (!strContent.empty())
    {
        strItem << nIndex;
        m_pListCtrl->InsertItem(nIndex, strItem.c_str());

        strItem.clear();
        strContent >> strItem;
        m_pListCtrl->SetItemText(nIndex, 1, strItem.c_str());

        strItem.clear();
        nIndex++;
    }

    return 1;
}

//////////////////////////////////////////////////////////////////////////
// Data loader for CTreeCtrl.
unsigned long CTreeCtrlDataLoader::Serialize(CYKTxtArchive& ar)
{
    if (!ar.IsLoading())
    {
        ASSERT(0);
        return 0;
    }

    if (NULL == m_pTreeCtrl)
    {
        ASSERT(0);
        return 0;
    }

    YKString strContent;
    ar.ReadEntireText(strContent);
    if (strContent.empty())
    {
        return 1;
    }

    strContent += _YKT('\n');

    YKString  strItem;
    YKString  strKeyWords = _YKT("[SubItemTag]");
    HTREEITEM hSubItem    = NULL;
    while (!strContent.empty())
    {
        strContent >> strItem;

        if (0 == _tcsncmp(strItem.c_str(), strKeyWords.c_str(), strKeyWords.length()))
        {
            strItem.replace(0, strKeyWords.length(), _YKT(""));
            hSubItem = m_pTreeCtrl->InsertItem(strItem.c_str());
        }
        else
        {
            if (NULL != hSubItem)
            {
                m_pTreeCtrl->InsertItem(strItem.c_str(), hSubItem);
            }
        }

        strItem.clear();
    }

    return 1;
}
