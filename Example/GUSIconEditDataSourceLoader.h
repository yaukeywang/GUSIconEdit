/********************************************************************
	created : 2013/04/25
	filename: GUSIconEditDataSourceLoader.h
	author  : Yaukey(yaukeywang@gmail.com)
	purpose : Classes for loading data source to data set control.
              All rights reserved.
    Licence : The Code Project Open License (CPOL)
              http://www.codeproject.com/info/cpol10.aspx
*********************************************************************/

#ifndef __GUS_ICON_EDIT_DATA_SOURCE_LOADER_H__
#define __GUS_ICON_EDIT_DATA_SOURCE_LOADER_H__

#include "YKArchive.h"

//////////////////////////////////////////////////////////////////////////
// Data loader for CListBox.
class CListBoxDataLoader : public CYKTxtArchiveSerializer
{
public:
    CListBoxDataLoader(LPCTSTR szFileName, int nOpenMode) : CYKTxtArchiveSerializer(szFileName, nOpenMode), m_pListBox(NULL) {}
    ~CListBoxDataLoader() {}

protected:
    virtual unsigned long Serialize(CYKTxtArchive& ar);

public:
    CListBox* m_pListBox;
};

//////////////////////////////////////////////////////////////////////////
// Data loader for CListCtrl.
class CListCtrlDataLoader : public CYKTxtArchiveSerializer
{
public:
    CListCtrlDataLoader(LPCTSTR szFileName, int nOpenMode) : CYKTxtArchiveSerializer(szFileName, nOpenMode), m_pListCtrl(NULL) {}
    ~CListCtrlDataLoader() {}

protected:
    virtual unsigned long Serialize(CYKTxtArchive& ar);

public:
    CListCtrl* m_pListCtrl;
};

//////////////////////////////////////////////////////////////////////////
// Data loader for CTreeCtrl.
class CTreeCtrlDataLoader : public CYKTxtArchiveSerializer
{
public:
    CTreeCtrlDataLoader(LPCTSTR szFileName, int nOpenMode) : CYKTxtArchiveSerializer(szFileName, nOpenMode), m_pTreeCtrl(NULL) {}
    ~CTreeCtrlDataLoader() {}

protected:
    virtual unsigned long Serialize(CYKTxtArchive& ar);

public:
    CTreeCtrl* m_pTreeCtrl;
};

#endif // __GUS_ICON_EDIT_DATA_SOURCE_LOADER_H__
