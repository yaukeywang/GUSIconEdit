// ***************************************************************
//  YKArchive.cpp   version:  1.0   ¡¤  date: 2010/08/28
//                  version:  1.1   .  date: 2011/03/24 
//                                     Add new open mode and add
//                                     methods for setting setting
//                                     file position.
//                  version   1.2   .  date: 2011/4/25
//                                     Make mbcs and unicode
//                                     methods separately.
//  -------------------------------------------------------------
//  Author: Yaukey
//  -------------------------------------------------------------
//  Copyright (C) 2010 - All Rights Reserved
// ***************************************************************
//  Description: file reading and writing support
// ***************************************************************
//  Licence : The Code Project Open License (CPOL)
//            http://www.codeproject.com/info/cpol10.aspx
// ***************************************************************

#include "stdafx.h"
#include "YKArchive.h"

//////////////////////////////////////////////////////////////////////////
// Some helper definition and functions.

// Buffer size.
const unsigned short YK_BUF_SIZE = 8;

// Check string length.
#define YK_CHECK_STR_LEN(cur, src, len)	\
{if((cur) - (src) >= (len) - 1) {*((src) + (len) - 1) = 0; return (src);}}

// Eat param - parse segments int a string.(multi-byte)
static YKLPCSTR YKEatParam(YKLPSTR pSeg, int nLen, YKStringA& strSrc)
{
	int n = (int)strSrc.find_first_of(" \t\r\n");
	if ((n < 0) || strSrc.empty())
	{
		*pSeg = 0;
		return pSeg;
	}

	YKLPCSTR p = strSrc.c_str();
	YKLPSTR  q = pSeg;
	while ((q - pSeg < n) && (q - pSeg < nLen - 1))
	{
		*q++ = *p++;
	}

	*q = 0;

	strSrc.erase(0, strSrc.find_first_not_of(" \t\r\n", n));
	return pSeg;
}

// Eat param - parse segments int a string.(unicode)
static YKLPCWSTR YKEatParam(YKLPWSTR pSeg, int nLen, YKStringW& strSrc)
{
	int n = (int)strSrc.find_first_of(L" \t\r\n");
	if ((n < 0) || strSrc.empty())
	{
		*pSeg = 0;
		return pSeg;
	}

	YKLPCWSTR p = strSrc.c_str();
	YKLPWSTR  q = pSeg;
	while ((q - pSeg < n) && (q - pSeg < nLen - 1))
	{
		*q++ = *p++;
	}

	*q = 0;

	strSrc.erase(0, strSrc.find_first_not_of(L" \t\r\n", n));
	return pSeg;
}

// Convert double number to a string.(mbcs)
static YKLPCSTR YKDoubleToStr(YKLPSTR pDst, int nBufLen, double fVal, int nPrec)
{
	if (nBufLen <= 0)
	{
		*pDst = 0;
		return pDst;
	}

	if (nPrec <= 0)
	{
		nPrec = 6;
	}
	
	if (nPrec >= 15)
	{
		nPrec = 15;
	}

	YKLPSTR pRDst = pDst;
	if (fVal < 0.0f)
	{
		fVal = -fVal;
		*pRDst++ = '-';
	}

	__int64 n64Val = (__int64)fVal;
	if (n64Val > 0)
	{
		YKCHAR szBuf[YK_BUF_SIZE * 8];
		_i64toa(n64Val, szBuf, 10);

		YKLPCSTR pSrc = szBuf;
		while ((0 != (*pRDst++ = *pSrc++)) && (pRDst - pDst < nBufLen - 1))
		{}

		YK_CHECK_STR_LEN(pRDst, pDst, nBufLen);
		*(--pRDst) = '.';
		pRDst++;
	}
	else
	{
		*pRDst++ = '0';
		YK_CHECK_STR_LEN(pRDst, pDst, nBufLen);
		*pRDst++ = '.';
		YK_CHECK_STR_LEN(pRDst, pDst, nBufLen);
	}

	fVal -= (double)n64Val;
	int nNum = 0;
	for (int i = 0; i < nPrec; i++)
	{
		fVal *= 10.0f;
		nNum = (int)fVal;
		*pRDst++ = (YKCHAR)(nNum + 48);
		YK_CHECK_STR_LEN(pRDst, pDst, nBufLen);
		fVal -= (double)nNum;
	}

	*pRDst = 0;
	return pDst;
}

// Convert double number to a string.(unicode)
static YKLPCWSTR YKDoubleToStr(YKLPWSTR pDst, int nBufLen, double fVal, int nPrec)
{
	if (nBufLen <= 0)
	{
		*pDst = 0;
		return pDst;
	}

	if (nPrec <= 0)
	{
		nPrec = 6;
	}

	if (nPrec >= 15)
	{
		nPrec = 15;
	}

	YKLPWSTR pRDst = pDst;
	if (fVal < 0.0f)
	{
		fVal = -fVal;
		*pRDst++ = L'-';
	}

	__int64 n64Val = (__int64)fVal;
	if (n64Val > 0)
	{
		YKWCHAR szBuf[YK_BUF_SIZE * 8];
		_i64tow(n64Val, szBuf, 10);

		YKLPCWSTR pSrc = szBuf;
		while ((0 != (*pRDst++ = *pSrc++)) && (pRDst - pDst < nBufLen - 1))
		{}

		YK_CHECK_STR_LEN(pRDst, pDst, nBufLen);
		*(--pRDst) = L'.';
		pRDst++;
	}
	else
	{
		*pRDst++ = L'0';
		YK_CHECK_STR_LEN(pRDst, pDst, nBufLen);
		*pRDst++ = L'.';
		YK_CHECK_STR_LEN(pRDst, pDst, nBufLen);
	}

	fVal -= (double)n64Val;
	int nNum = 0;
	for (int i = 0; i < nPrec; i++)
	{
		fVal *= 10.0f;
		nNum = (int)fVal;
		*pRDst++ = (YKWCHAR)(nNum + 48);
		YK_CHECK_STR_LEN(pRDst, pDst, nBufLen);
		fVal -= (double)nNum;
	}

	*pRDst = 0;
	return pDst;
}

// Write or read byte to or from a string.

YKStringA& operator<<(YKStringA& str, unsigned char yVal)
{
	YKCHAR szBuf[YK_BUF_SIZE];
	_itoa(yVal, szBuf, 10);
	str += szBuf;

	return str;
}

YKStringW& operator<<(YKStringW& str, unsigned char yVal)
{
	YKWCHAR szBuf[YK_BUF_SIZE];
	_itow(yVal, szBuf, 10);
	str += szBuf;

	return str;
}

YKStringA& operator>>(YKStringA& str, unsigned char& yVal)
{
	YKCHAR szBuf[YK_BUF_SIZE * 8];
	if (0 != (*YKEatParam(szBuf, YK_BUF_SIZE * 8, str)))
	{
		yVal = (unsigned char)atoi(szBuf);
	}

	return str;
}

YKStringW& operator>>(YKStringW& str, unsigned char& yVal)
{
	YKWCHAR szBuf[YK_BUF_SIZE * 8];
	if (0 != (*YKEatParam(szBuf, YK_BUF_SIZE * 8, str)))
	{
		yVal = (unsigned char)_wtoi(szBuf);
	}

	return str;
}

// Write or read int to or from a string.

YKStringA& operator<<(YKStringA& str, int nVal)
{
	YKCHAR szBuf[YK_BUF_SIZE * 2];
	_itoa(nVal, szBuf, 10);
	str += szBuf;

	return str;
}

YKStringW& operator<<(YKStringW& str, int nVal)
{
	YKWCHAR szBuf[YK_BUF_SIZE * 2];
	_itow(nVal, szBuf, 10);
	str += szBuf;

	return str;
}

YKStringA& operator>>(YKStringA& str, int& nVal)
{
	YKCHAR szBuf[YK_BUF_SIZE * 8];
	if (0 != (*YKEatParam(szBuf, YK_BUF_SIZE * 8, str)))
	{
		nVal = atoi(szBuf);
	}

	return str;
}

YKStringW& operator>>(YKStringW& str, int& nVal)
{
	YKWCHAR szBuf[YK_BUF_SIZE * 8];
	if (0 != (*YKEatParam(szBuf, YK_BUF_SIZE * 8, str)))
	{
		nVal = _wtoi(szBuf);
	}

	return str;
}

// Write or read long to or from a string.

YKStringA& operator<<(YKStringA& str, long lVal)
{
	YKCHAR szBuf[YK_BUF_SIZE * 2];
	_ltoa(lVal, szBuf, 10);
	str += szBuf;

	return str;
}

YKStringW& operator<<(YKStringW& str, long lVal)
{
	YKWCHAR szBuf[YK_BUF_SIZE * 2];
	_ltow(lVal, szBuf, 10);
	str += szBuf;

	return str;
}

YKStringA& operator>>(YKStringA& str, long& lVal)
{
	YKCHAR szBuf[YK_BUF_SIZE * 8];
	if (0 != (*YKEatParam(szBuf, YK_BUF_SIZE * 8, str)))
	{
		lVal = atol(szBuf);
	}

	return str;
}

YKStringW& operator>>(YKStringW& str, long& lVal)
{
	YKWCHAR szBuf[YK_BUF_SIZE * 8];
	if (0 != (*YKEatParam(szBuf, YK_BUF_SIZE * 8, str)))
	{
		lVal = _wtol(szBuf);
	}

	return str;
}

// Write or read unsigned long(DWORD) to or from a string.

YKStringA& operator<<(YKStringA& str, unsigned long ulVal)
{
	YKCHAR szBuf[YK_BUF_SIZE * 8];
	_ultoa(ulVal, szBuf, 10);
	str += szBuf;

	return str;
}

YKStringW& operator<<(YKStringW& str, unsigned long ulVal)
{
	YKWCHAR szBuf[YK_BUF_SIZE * 8];
	_ultow(ulVal, szBuf, 10);
	str += szBuf;

	return str;
}

YKStringA& operator>>(YKStringA& str, unsigned long& ulVal)
{
	YKCHAR szBuf[YK_BUF_SIZE * 8];
	if (0 != (*YKEatParam(szBuf, YK_BUF_SIZE * 8, str)))
	{
		ulVal = (unsigned long)_atoi64(szBuf);
	}

	return str;
}

YKStringW& operator>>(YKStringW& str, unsigned long& ulVal)
{
	YKWCHAR szBuf[YK_BUF_SIZE * 8];
	if (0 != (*YKEatParam(szBuf, YK_BUF_SIZE * 8, str)))
	{
		ulVal = (unsigned long)_wtoi64(szBuf);
	}

	return str;
}

// Write or read __int64 to or from a string.

YKStringA& operator<<(YKStringA& str, __int64 n64Val)
{
	YKCHAR szBuf[YK_BUF_SIZE * 8];
	_i64toa(n64Val, szBuf, 10);
	str += szBuf;

	return str;
}

YKStringW& operator<<(YKStringW& str, __int64 n64Val)
{
	YKWCHAR szBuf[YK_BUF_SIZE * 8];
	_i64tow(n64Val, szBuf, 10);
	str += szBuf;

	return str;
}

YKStringA& operator>>(YKStringA& str, __int64& n64Val)
{
	YKCHAR szBuf[YK_BUF_SIZE * 8];
	if (0 != (*YKEatParam(szBuf, YK_BUF_SIZE * 8, str)))
	{
		n64Val = _atoi64(szBuf);
	}

	return str;
}

YKStringW& operator>>(YKStringW& str, __int64& n64Val)
{
	YKWCHAR szBuf[YK_BUF_SIZE * 8];
	if (0 != (*YKEatParam(szBuf, YK_BUF_SIZE * 8, str)))
	{
		n64Val = _wtoi64(szBuf);
	}

	return str;
}

// Write or read float to or from a string.

YKStringA& operator<<(YKStringA& str, float fVal)
{
	YKCHAR szBuf[YK_BUF_SIZE * 8];
	YKDoubleToStr(szBuf, YK_BUF_SIZE * 8, fVal, 8);
	str += szBuf;

	return str;
}

YKStringW& operator<<(YKStringW& str, float fVal)
{
	YKWCHAR szBuf[YK_BUF_SIZE * 8];
	YKDoubleToStr(szBuf, YK_BUF_SIZE * 8, fVal, 8);
	str += szBuf;

	return str;
}

YKStringA& operator>>(YKStringA& str, float& fVal)
{
	YKCHAR szBuf[YK_BUF_SIZE * 8];
	if (0 != (*YKEatParam(szBuf, YK_BUF_SIZE * 8, str)))
	{
		fVal = (float)atof(szBuf);
	}

	return str;
}

YKStringW& operator>>(YKStringW& str, float& fVal)
{
	YKWCHAR szBuf[YK_BUF_SIZE * 8];
	if (0 != (*YKEatParam(szBuf, YK_BUF_SIZE * 8, str)))
	{
		fVal = (float)_wtof(szBuf);
	}

	return str;
}

// Write or read double to or from a string.

YKStringA& operator<<(YKStringA& str, double fVal)
{
	YKCHAR szBuf[YK_BUF_SIZE * 8];
	YKDoubleToStr(szBuf, YK_BUF_SIZE * 8, fVal, 15);
	str += szBuf;

	return str;
}

YKStringW& operator<<(YKStringW& str, double fVal)
{
	YKWCHAR szBuf[YK_BUF_SIZE * 8];
	YKDoubleToStr(szBuf, YK_BUF_SIZE * 8, fVal, 15);
	str += szBuf;

	return str;
}

YKStringA& operator>>(YKStringA& str, double& fVal)
{
	YKCHAR szBuf[YK_BUF_SIZE * 8];
	if (0 != (*YKEatParam(szBuf, YK_BUF_SIZE * 8, str)))
	{
		fVal = atof(szBuf);
	}

	return str;
}

YKStringW& operator>>(YKStringW& str, double& fVal)
{
	YKWCHAR szBuf[YK_BUF_SIZE * 8];
	if (0 != (*YKEatParam(szBuf, YK_BUF_SIZE * 8, str)))
	{
		fVal = _wtof(szBuf);
	}

	return str;
}

// Write a single character.

YKStringA& operator<<(YKStringA& str, YKCHAR ch)
{
	str += ch;
	return str;
}

YKStringW& operator<<(YKStringW& str, YKWCHAR ch)
{
	str += ch;
	return str;
}

// Write or read a string value to or from another string.

YKStringA& operator<<(YKStringA& str, YKLPCSTR pStr)
{
	str += pStr;
	return str;
}

YKStringW& operator<<(YKStringW& str, YKLPCWSTR pStr)
{
	str += pStr;
	return str;
}

YKStringA& operator>>(YKStringA& str, YKStringA& strOut)
{
	YKCHAR szBuf[YK_BUF_SIZE * 10];
	if (0 != (*YKEatParam(szBuf, YK_BUF_SIZE * 8, str)))
	{
		strOut += szBuf;
	}

	return str;
}

YKStringW& operator>>(YKStringW& str, YKStringW& strOut)
{
	YKWCHAR szBuf[YK_BUF_SIZE * 10];
	if (0 != (*YKEatParam(szBuf, YK_BUF_SIZE * 8, str)))
	{
		strOut += szBuf;
	}

	return str;
}

// Read string from strSrc to strDest until matching a character "chStop".

YKLPCSTR YKReadStringUntilChar(YKStringA& strDest, YKCHAR chStop, YKStringA& strSrc)
{ 
	int nSize = (int)strSrc.length();

	YKLPCSTR pSrc   = strSrc.c_str();
	YKLPCSTR pCount = pSrc;

	while ((pCount - pSrc < nSize) && (*pCount != chStop))
	{
		strDest += *pCount++;
	}

	strSrc.erase(0, pCount - pSrc + 1);
	return strDest.c_str();
}

YKLPCWSTR YKReadStringUntilChar(YKStringW& strDest, YKWCHAR chStop, YKStringW& strSrc)
{ 
	int nSize = (int)strSrc.length();

	YKLPCWSTR pSrc   = strSrc.c_str();
	YKLPCWSTR pCount = pSrc;

	while ((pCount - pSrc < nSize) && (*pCount != chStop))
	{
		strDest += *pCount++;
	}

	strSrc.erase(0, pCount - pSrc + 1);
	return strDest.c_str();
}

//////////////////////////////////////////////////////////////////////////
// YKArchive base class.

CYKArchive::CYKArchive() : 
	m_pFile(NULL),
	m_nOpenMode(OM_INVALID),
	m_dwFileSize(0),
	m_bLoading(true),
	m_bOpen(false)
{
}

CYKArchive::CYKArchive(YKLPCSTR szFileName, int nOpenMode) : 
	m_pFile(NULL),
	m_dwFileSize(0),
	m_bOpen(false)
{
	m_szFileNameA = szFileName;
	m_nOpenMode   = nOpenMode;
	m_bLoading    = (OM_READONLY == nOpenMode) ? true : false;
}

CYKArchive::CYKArchive(YKLPCWSTR szFileName, int nOpenMode) :
	m_pFile(NULL),
	m_dwFileSize(0),
	m_bOpen(false)
{
	m_szFileNameW = szFileName;
	m_nOpenMode   = nOpenMode;
	m_bLoading    = (OM_READONLY == nOpenMode) ? true : false;
}

CYKArchive::~CYKArchive()
{
	if ((NULL != m_pFile) || m_bOpen)
	{
		fclose(m_pFile);
		m_pFile = NULL;
	}
}

bool CYKArchive::Open()
{
	if (m_szFileNameA.empty() && m_szFileNameW.empty())
	{
		return false;
	}

	// Set file exist file open mode str.(mbcs)
	YKLPCSTR szReadA       = NULL;
	YKLPCSTR szWriteA      = NULL;
	YKLPCSTR szAppendA     = NULL;
	YKLPCSTR szReadPlusA   = NULL;
	YKLPCSTR szWritePlusA  = NULL;
	YKLPCSTR szAppendPlusA = NULL;
	SetOpenModeString(&szReadA, &szWriteA, &szAppendA, &szReadPlusA, &szWritePlusA, &szAppendPlusA);
	if (
		(NULL == szReadA)      || 
		(NULL == szWriteA)     || 
		(NULL == szAppendA)    || 
		(NULL == szReadPlusA)  || 
		(NULL == szWritePlusA) ||
		(NULL == szAppendPlusA)
		)
	{
		return false;
	}

	// Set file exist file open mode str.(unicode)
	YKLPCWSTR szReadW       = NULL;
	YKLPCWSTR szWriteW      = NULL;
	YKLPCWSTR szAppendW     = NULL;
	YKLPCWSTR szReadPlusW   = NULL;
	YKLPCWSTR szWritePlusW  = NULL;
	YKLPCWSTR szAppendPlusW = NULL;
	SetOpenModeString(&szReadW, &szWriteW, &szAppendW, &szReadPlusW, &szWritePlusW, &szAppendPlusW);
	if (
		(NULL == szReadW)      || 
		(NULL == szWriteW)     || 
		(NULL == szAppendW)    || 
		(NULL == szReadPlusW)  || 
		(NULL == szWritePlusW) ||
		(NULL == szAppendPlusW)
		)
	{
		return false;
	}

	YKLPCSTR  szOMA = NULL;
	YKLPCWSTR szOMW = NULL;
	switch (m_nOpenMode)
	{
	case OM_READONLY:
		szOMA      = szReadA;
		szOMW      = szReadW;
		m_bLoading = true;
		break;
	case OM_WRITEONLY:
		szOMA      = szWriteA;
		szOMW      = szWriteW;
		m_bLoading = false;
		break;
	case OM_APPEND:
		szOMA      = szAppendA;
		szOMW      = szAppendW;
		m_bLoading = false;
		break;
	case OM_READPLUS:
		szOMA      = szReadPlusA;
		szOMW      = szReadPlusW;
		m_bLoading = true;
		break;
	case OM_WRITEPLUS:
		szOMA      = szWritePlusA;
		szOMW      = szWritePlusW;
		m_bLoading = false;
		break;
	case OM_APPENDPLUS:
		szOMA      = szAppendPlusA;
		szOMW      = szAppendPlusW;
		m_bLoading = false;
		break;
	default:
		return false;
	}

	if (!m_szFileNameA.empty())
	{
		if (NULL == (m_pFile = fopen(m_szFileNameA.c_str(), szOMA)))
		{
			return false;
		}
	}
	else
	{
		if (NULL == (m_pFile = _wfopen(m_szFileNameW.c_str(), szOMW)))
		{
			return false;
		}
	}

	m_bOpen = true;
	if (OM_READONLY == m_nOpenMode)
	{
		fseek(m_pFile, 0L, SEEK_END);
		long nSize = ftell(m_pFile);
		m_dwFileSize = (nSize > 0) ? nSize : 0;
		rewind(m_pFile);
	}

	return true;
}

bool CYKArchive::Close()
{
	if ((NULL != m_pFile) || m_bOpen)
	{
		fclose(m_pFile);
		m_pFile = NULL;
		m_bOpen = false;
	}

	return (NULL == m_pFile);
}

bool CYKArchive::IsValid() const 
{
	return (m_bOpen && (NULL != m_pFile));
}

bool CYKArchive::IsLoading() const 
{
	return m_bLoading;
}

void CYKArchive::Rewind()
{
	if (NULL != m_pFile)
	{
		rewind(m_pFile);
	}
}

int CYKArchive::Seek(long lLength, int nOrigin)
{
	if (NULL == m_pFile)
	{
		return -1;
	}

	switch (nOrigin)
	{
	case SM_SEEK_BEGIN:
		return fseek(m_pFile, lLength, SEEK_SET);
	case SM_SEEK_CURRENT:
		return fseek(m_pFile, lLength, SEEK_CUR);
	case SM_SEEK_END:
	    return fseek(m_pFile, lLength, SEEK_END);
	default:
	    break;
	}

	return -1;
}

long CYKArchive::Tell()
{
	if (NULL == m_pFile)
	{
		return 0;
	}

	return ftell(m_pFile);
}

bool CYKArchive::IsEndOfFile() const 
{
	return (0 != feof(m_pFile));
}

YKLPCSTR CYKArchive::GetFileNameA() const 
{
	return m_szFileNameA.c_str();
}

YKLPCWSTR CYKArchive::GetFileNameW() const 
{
	return m_szFileNameW.c_str();
}

void CYKArchive::SetFileNameA(YKLPCSTR szFileName) 
{
	m_szFileNameA = szFileName;
	m_szFileNameW.clear();
}

void CYKArchive::SetFileNameW(YKLPCWSTR szFileName)
{
	m_szFileNameW = szFileName;
	m_szFileNameA.clear();
}

void CYKArchive::SetOpenMode(int nOpenMode)
{
	m_nOpenMode = nOpenMode;
	m_bLoading  = (OM_READONLY == nOpenMode) ? true : false;
}

int CYKArchive::GetOpenMode() const 
{
	return m_nOpenMode;
}

unsigned long CYKArchive::GetFileSize() const 
{
	return m_dwFileSize;
}

//////////////////////////////////////////////////////////////////////////
// Bin archive.

YKLPCSTR CYKBinArchive::s_szOMReadA       = "rb";
YKLPCSTR CYKBinArchive::s_szOMWriteA      = "wb";
YKLPCSTR CYKBinArchive::s_szOMAppendA     = "ab";
YKLPCSTR CYKBinArchive::s_szOMReadPlusA   = "rb+";
YKLPCSTR CYKBinArchive::s_szOMWritePlusA  = "wb+";
YKLPCSTR CYKBinArchive::s_szOMAppendPlusA = "ab+";

YKLPCWSTR CYKBinArchive::s_szOMReadW       = L"rb";
YKLPCWSTR CYKBinArchive::s_szOMWriteW      = L"wb";
YKLPCWSTR CYKBinArchive::s_szOMAppendW     = L"ab";
YKLPCWSTR CYKBinArchive::s_szOMReadPlusW   = L"rb+";
YKLPCWSTR CYKBinArchive::s_szOMWritePlusW  = L"wb+";
YKLPCWSTR CYKBinArchive::s_szOMAppendPlusW = L"ab+";

const unsigned long CYKBinArchive::s_dwReadStrBufSize = 512;

CYKBinArchive::CYKBinArchive()
{
}

CYKBinArchive::CYKBinArchive(YKLPCSTR szFileName, int nOpenMode) :
	CYKArchive(szFileName, nOpenMode)
{
}

CYKBinArchive::CYKBinArchive(YKLPCWSTR szFileName, int nOpenMode) :
	CYKArchive(szFileName, nOpenMode)
{
	
}

CYKBinArchive::~CYKBinArchive()
{
}

void CYKBinArchive::SetOpenModeString(
	YKLPCSTR* ppOMRead, 
	YKLPCSTR* ppOMWrite, 
	YKLPCSTR* ppOMAppend,
	YKLPCSTR* ppOMReadPlus,
	YKLPCSTR* ppOMWritePlus, 
	YKLPCSTR* ppOMAppendPlus
	)
{
	*ppOMRead       = s_szOMReadA;
	*ppOMWrite      = s_szOMWriteA;
	*ppOMAppend     = s_szOMAppendA;
	*ppOMReadPlus   = s_szOMReadPlusA;
	*ppOMWritePlus  = s_szOMWritePlusA;
	*ppOMAppendPlus = s_szOMAppendPlusA;
}

void CYKBinArchive::SetOpenModeString(
	YKLPCWSTR* ppOMRead, 
	YKLPCWSTR* ppOMWrite, 
	YKLPCWSTR* ppOMAppend,
	YKLPCWSTR* ppOMReadPlus,
	YKLPCWSTR* ppOMWritePlus,
	YKLPCWSTR* ppOMAppendPlus
	)
{
	*ppOMRead       = s_szOMReadW;
	*ppOMWrite      = s_szOMWriteW;
	*ppOMAppend     = s_szOMAppendW;
	*ppOMReadPlus   = s_szOMReadPlusW;
	*ppOMWritePlus  = s_szOMWritePlusW;
	*ppOMAppendPlus = s_szOMAppendPlusW;
}

unsigned long CYKBinArchive::WriteString(const YKStringA& str)
{
	unsigned long dwLen = (unsigned long)str.length();
	operator<<(dwLen);
	WriteArray(str.c_str(), dwLen);

	return (dwLen * sizeof(YKCHAR));
}

unsigned long CYKBinArchive::WriteString(const YKStringW& str)
{
	unsigned long dwLen = (unsigned long)str.length();
	operator<<(dwLen);
	WriteArray(str.c_str(), dwLen);

	return (dwLen * sizeof(YKWCHAR));
}

unsigned long CYKBinArchive::ReadString(YKStringA& str)
{
	YKCHAR szBuf[s_dwReadStrBufSize];

	unsigned long dwLen = 0;
	operator>>(dwLen);
	if (dwLen > m_dwFileSize)
	{
		ASSERT(0);
		return 0;
	}

	if (dwLen < s_dwReadStrBufSize)
	{
		ReadArray(szBuf, dwLen);
		szBuf[dwLen] = 0;
		str = szBuf;
	}
	else
	{
		unsigned long dwTmpLen = dwLen;
		while (dwTmpLen >= s_dwReadStrBufSize)
		{
			ReadArray(szBuf, s_dwReadStrBufSize - 1);
			szBuf[s_dwReadStrBufSize - 1] = 0;
			str += szBuf;

			dwTmpLen -= (s_dwReadStrBufSize - 1);
		}

		if (dwTmpLen > 0)
		{
			ReadArray(szBuf, dwTmpLen);
			szBuf[dwTmpLen] = 0;
			str += szBuf;
		}
	}

	return (dwLen * sizeof(YKCHAR));
}

unsigned long CYKBinArchive::ReadString(YKStringW& str)
{
	YKWCHAR szBuf[s_dwReadStrBufSize];

	unsigned long dwLen = 0;
	operator>>(dwLen);
	if (dwLen > m_dwFileSize)
	{
		ASSERT(0);
		return 0;
	}

	if (dwLen < s_dwReadStrBufSize)
	{
		ReadArray(szBuf, dwLen);
		szBuf[dwLen] = 0;
		str = szBuf;
	}
	else
	{
		unsigned long dwTmpLen = dwLen;
		while (dwTmpLen >= s_dwReadStrBufSize)
		{
			ReadArray(szBuf, s_dwReadStrBufSize - 1);
			szBuf[s_dwReadStrBufSize - 1] = 0;
			str += szBuf;

			dwTmpLen -= (s_dwReadStrBufSize - 1);
		}

		if (dwTmpLen > 0)
		{
			ReadArray(szBuf, dwTmpLen);
			szBuf[dwTmpLen] = 0;
			str += szBuf;
		}
	}

	return (dwLen * sizeof(YKWCHAR));
}

//////////////////////////////////////////////////////////////////////////
// Txt archive.

YKLPCSTR CYKTxtArchive::s_szOMReadA       = "r";
YKLPCSTR CYKTxtArchive::s_szOMWriteA      = "w";
YKLPCSTR CYKTxtArchive::s_szOMAppendA     = "a";
YKLPCSTR CYKTxtArchive::s_szOMReadPlusA   = "r+";
YKLPCSTR CYKTxtArchive::s_szOMWritePlusA  = "w+";
YKLPCSTR CYKTxtArchive::s_szOMAppendPlusA = "a+";

YKLPCWSTR CYKTxtArchive::s_szOMReadW       = L"r";
YKLPCWSTR CYKTxtArchive::s_szOMWriteW      = L"w";
YKLPCWSTR CYKTxtArchive::s_szOMAppendW     = L"a";
YKLPCWSTR CYKTxtArchive::s_szOMReadPlusW   = L"r+";
YKLPCWSTR CYKTxtArchive::s_szOMWritePlusW  = L"w+";
YKLPCWSTR CYKTxtArchive::s_szOMAppendPlusW = L"a+";

const unsigned long CYKTxtArchive::s_dwReadStrBufSize = 512;

CYKTxtArchive::CYKTxtArchive()
{
}

CYKTxtArchive::CYKTxtArchive(YKLPCSTR szFileName, int nOpenMode) :
	CYKArchive(szFileName, nOpenMode)
{
}

CYKTxtArchive::CYKTxtArchive(YKLPCWSTR szFileName, int nOpenMode) :
	CYKArchive(szFileName, nOpenMode)
{
}

CYKTxtArchive::~CYKTxtArchive()
{
}

void CYKTxtArchive::SetOpenModeString(
	YKLPCSTR* ppOMRead, 
	YKLPCSTR* ppOMWrite, 
	YKLPCSTR* ppOMAppend,
	YKLPCSTR* ppOMReadPlus,
	YKLPCSTR* ppOMWritePlus,
	YKLPCSTR* ppOMAppendPlus
	)
{
	*ppOMRead       = s_szOMReadA;
	*ppOMWrite      = s_szOMWriteA;
	*ppOMAppend     = s_szOMAppendA;
	*ppOMReadPlus   = s_szOMReadPlusA;
	*ppOMWritePlus  = s_szOMWritePlusA;
	*ppOMAppendPlus = s_szOMAppendPlusA;
}

void CYKTxtArchive::SetOpenModeString(
	YKLPCWSTR* ppOMRead, 
	YKLPCWSTR* ppOMWrite, 
	YKLPCWSTR* ppOMAppend,
	YKLPCWSTR* ppOMReadPlus,
	YKLPCWSTR* ppOMWritePlus,
	YKLPCWSTR* ppOMAppendPlus
	)
{
	*ppOMRead       = s_szOMReadW;
	*ppOMWrite      = s_szOMWriteW;
	*ppOMAppend     = s_szOMAppendW;
	*ppOMReadPlus   = s_szOMReadPlusW;
	*ppOMWritePlus  = s_szOMWritePlusW;
	*ppOMAppendPlus = s_szOMAppendPlusW;
}

CYKTxtArchive& CYKTxtArchive::operator<<(YKCHAR ch)
{
	ASSERT(NULL != m_pFile);

	if (!m_bOpen)
	{
		ASSERT(0);
		return *this;
	}

	int nCh = fputc(ch, m_pFile);
	ASSERT(EOF != nCh);

	return *this;
}

CYKTxtArchive& CYKTxtArchive::operator<<(YKWCHAR ch)
{
	ASSERT(NULL != m_pFile);

	if (!m_bOpen)
	{
		ASSERT(0);
		return *this;
	}

	unsigned short nCh = fputwc(ch, m_pFile);
	ASSERT(WEOF != nCh);

	return *this;
}

CYKTxtArchive& CYKTxtArchive::operator>>(YKCHAR& ch)
{
	ASSERT(NULL != m_pFile);

	if (!m_bOpen)
	{
		ASSERT(0);
		return *this;
	}

	int nCh = fgetc(m_pFile);
	ch = (EOF != nCh) ? (YKCHAR)nCh : ch;
	ASSERT(EOF != nCh);

	return *this;
}

CYKTxtArchive& CYKTxtArchive::operator>>(YKWCHAR& ch)
{
	ASSERT(NULL != m_pFile);

	if (!m_bOpen)
	{
		ASSERT(0);
		return *this;
	}

	unsigned short nCh = fgetwc(m_pFile);
	ch = (WEOF != nCh) ? (YKWCHAR)nCh : ch;
	ASSERT(WEOF != nCh);

	return *this;
}

bool CYKTxtArchive::WriteString(YKLPCSTR pStr)
{
	ASSERT(NULL != m_pFile);

	if (!m_bOpen)
	{
		ASSERT(0);
		return false;
	}

	int nRes = fputs(pStr, m_pFile);
	return (EOF != nRes);
}

bool CYKTxtArchive::WriteString(YKLPCWSTR pStr)
{
	ASSERT(NULL != m_pFile);

	if (!m_bOpen)
	{
		ASSERT(0);
		return false;
	}

	int nRes = fputws(pStr, m_pFile);
	return (WEOF != nRes);
}

bool CYKTxtArchive::ReadString(YKStringA& str, int nLen)
{
	ASSERT(NULL != m_pFile);

	if (!m_bOpen)
	{
		ASSERT(0);
		return false;
	}

	if (nLen <= 0)
	{
		return false;
	}

	int     nCount = 0;
	YKCHAR ch;
	while (!feof(m_pFile) && (nCount < nLen))
	{
		ch = (YKCHAR)fgetc(m_pFile);
		str += ch;
		nCount++;
	}

	return true;
}

bool CYKTxtArchive::ReadString(YKStringW& str, int nLen)
{
	ASSERT(NULL != m_pFile);

	if (!m_bOpen)
	{
		ASSERT(0);
		return false;
	}

	if (nLen <= 0)
	{
		return false;
	}

	int     nCount = 0;
	YKWCHAR ch;
	while (!feof(m_pFile) && (nCount < nLen))
	{
		ch = (YKWCHAR)fgetwc(m_pFile);
		str += ch;
		nCount++;
	}

	return true;
}

void CYKTxtArchive::ReadLine(YKStringA& str)
{
	ASSERT(NULL != m_pFile);

	if (!m_bOpen)
	{
		ASSERT(0);
		return;
	}

	YKCHAR ch;
	while (!feof(m_pFile) && ('\n' != (ch = (YKCHAR)fgetc(m_pFile))))
	{
		str += ch;
	}
}

void CYKTxtArchive::ReadLine(YKStringW& str)
{
	ASSERT(NULL != m_pFile);

	if (!m_bOpen)
	{
		ASSERT(0);
		return;
	}

	YKWCHAR ch;
	while (!feof(m_pFile) && (L'\n' != (ch = (YKWCHAR)fgetwc(m_pFile))))
	{
		str += ch;
	}
}

bool CYKTxtArchive::ReadEntireText(YKStringA& str)
{
	ASSERT(NULL != m_pFile);

	if (!m_bOpen)
	{
		ASSERT(0);
		return false;
	}

	YKCHAR szBuf[s_dwReadStrBufSize];
	YKLPSTR pRes = fgets(szBuf, (int)s_dwReadStrBufSize, m_pFile);
	if (NULL == pRes)
	{
		return false;
	}

	str = szBuf;

	while (NULL != pRes)
	{
		pRes = fgets(szBuf, (int)s_dwReadStrBufSize, m_pFile);
		if (NULL == pRes)
		{
			return true;
		}

		str += szBuf;
	}

	return true;
}

bool CYKTxtArchive::ReadEntireText(YKStringW& str)
{
	ASSERT(NULL != m_pFile);

	if (!m_bOpen)
	{
		ASSERT(0);
		return false;
	}

	YKWCHAR szBuf[s_dwReadStrBufSize];
	YKLPWSTR pRes = fgetws(szBuf, (int)s_dwReadStrBufSize, m_pFile);
	if (NULL == pRes)
	{
		return false;
	}

	str = szBuf;

	while (NULL != pRes)
	{
		pRes = fgetws(szBuf, (int)s_dwReadStrBufSize, m_pFile);
		if (NULL == pRes)
		{
			return true;
		}

		str += szBuf;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
// Bin archive serializer usage base class.

CYKBinArchiveSerializer::CYKBinArchiveSerializer() : 
	m_nOpenMode(OM_WRITEONLY)
{
}

CYKBinArchiveSerializer::CYKBinArchiveSerializer(YKLPCSTR szFileName, int nOpenMode) : 
	m_szFileNameA(szFileName), 
	m_nOpenMode(nOpenMode)
{
}

CYKBinArchiveSerializer::CYKBinArchiveSerializer(YKLPCWSTR szFileName, int nOpenMode) :
	m_szFileNameW(szFileName), 
	m_nOpenMode(nOpenMode)
{
}

CYKBinArchiveSerializer::~CYKBinArchiveSerializer()
{
}

bool CYKBinArchiveSerializer::Go()
{
	CYKBinArchive arc;
	arc.SetOpenMode(m_nOpenMode);
	if (!m_szFileNameA.empty())
	{
		arc.SetFileNameA(m_szFileNameA.c_str());
	}
	else
	{
		arc.SetFileNameW(m_szFileNameW.c_str());
	}

	if (!arc.Open())
	{
		return false;
	}

	if (!arc.IsValid())
	{
		return false;
	}

	// Read or write file.
	unsigned long dwSize = Serialize(arc);
	arc.Close();

	return (dwSize > 0);
}

unsigned long CYKBinArchiveSerializer::Serialize(CYKBinArchive& ar)
{
	unsigned long dwSize = 0;
	if (ar.IsLoading())
	{
		dwSize = 1;
	}
	else
	{
		dwSize = 1;
	}

	return dwSize;
}

//////////////////////////////////////////////////////////////////////////
// Txt archive serializer usage base class.

CYKTxtArchiveSerializer::CYKTxtArchiveSerializer() : 
	m_nOpenMode(OM_WRITEONLY)
{
}

CYKTxtArchiveSerializer::CYKTxtArchiveSerializer(YKLPCSTR szFileName, int nOpenMode) : 
	m_szFileNameA(szFileName), 
	m_nOpenMode(nOpenMode) 
{
}

CYKTxtArchiveSerializer::CYKTxtArchiveSerializer(YKLPCWSTR szFileName, int nOpenMode) :
	m_szFileNameW(szFileName), 
	m_nOpenMode(nOpenMode) 
{
}

CYKTxtArchiveSerializer::~CYKTxtArchiveSerializer() 
{
}

bool CYKTxtArchiveSerializer::Go()
{
	CYKTxtArchive arc;
	arc.SetOpenMode(m_nOpenMode);
	if (!m_szFileNameA.empty())
	{
		arc.SetFileNameA(m_szFileNameA.c_str());
	}
	else
	{
		arc.SetFileNameW(m_szFileNameW.c_str());
	}

	if (!arc.Open())
	{
		return false;
	}

	if (!arc.IsValid())
	{
		return false;
	}

	// Read or write file.
	unsigned long dwSize = Serialize(arc);
	arc.Close();

	return (dwSize > 0);
}

unsigned long CYKTxtArchiveSerializer::Serialize(CYKTxtArchive& ar)
{
	unsigned long dwSize = 0;
	if (ar.IsLoading())
	{
		dwSize = 1;
	}
	else
	{
		dwSize = 1;
	}

	return dwSize;
}
