// ***************************************************************
//  YKArchive.h   version:  1.0   ¡¤  date: 2010/08/28
//                version:  1.1   .  date: 2011/03/24 
//                                   Add new open mode and add
//                                   methods for setting setting
//                                   file position.
//                version   1.2   .  date: 2011/4/25
//                                   Make mbcs and unicode
//                                   methods separately.
//  -------------------------------------------------------------
//  Author: Yaukey
//  -------------------------------------------------------------
//  Copyright (C) 2010 - All Rights Reserved
// ***************************************************************
//  Description: File reading and writing support.
// ***************************************************************
//  Licence : The Code Project Open License (CPOL)
//            http://www.codeproject.com/info/cpol10.aspx
// ***************************************************************

#ifndef __YKARCHIVE_H__
#define __YKARCHIVE_H__

//////////////////////////////////////////////////////////////////////////
// Warnings.

// Disable msvc secure warnings.
#if defined(WIN32) && defined(_MSC_VER)
#if !defined(_CRT_SECURE_NO_WARNINGS) && (_MSC_VER >= 1200)
#define _CRT_SECURE_NO_WARNINGS
//#pragma warning(disable:4996)
#endif
#endif

//////////////////////////////////////////////////////////////////////////
// Headers.

#include <stdio.h>
#include <stdlib.h>
#include <string>

//////////////////////////////////////////////////////////////////////////
// Assert.

#ifndef ASSERT
#include <assert.h>
#define ASSERT assert
#endif

//////////////////////////////////////////////////////////////////////////
// String.

// Base mbcs string type.
typedef       char  YKCHAR;
typedef       char* YKLPSTR;
typedef const char* YKLPCSTR;

// Base unicode string type.
typedef       wchar_t  YKWCHAR;
typedef       wchar_t* YKLPWSTR;
typedef const wchar_t* YKLPCWSTR;

// Base std string type.
typedef std::string  YKStringA;
typedef std::wstring YKStringW;

#ifndef _UNICODE
typedef YKCHAR    YKTCHAR;
typedef YKLPSTR   YKLPTSTR;
typedef YKLPCSTR  YKLPCTSTR;
typedef YKStringA YKString;
#define _YKT(x)   x
#else
typedef YKWCHAR   YKTCHAR;
typedef YKLPWSTR  YKLPTSTR;
typedef YKLPCWSTR YKLPCTSTR;
typedef YKStringW YKString;
#define _YKT(x)   L##x
#endif

//////////////////////////////////////////////////////////////////////////
// Some helper definition and functions.

// Write or read byte to or from a string.

YKStringA& operator<<(YKStringA& str, unsigned char yVal);
YKStringW& operator<<(YKStringW& str, unsigned char yVal);
YKStringA& operator>>(YKStringA& str, unsigned char& yVal);
YKStringW& operator>>(YKStringW& str, unsigned char& yVal);

// Write or read int to or from a string.

YKStringA& operator<<(YKStringA& str, int nVal);
YKStringW& operator<<(YKStringW& str, int nVal);
YKStringA& operator>>(YKStringA& str, int& nVal);
YKStringW& operator>>(YKStringW& str, int& nVal);

// Write or read long to or from a string.

YKStringA& operator<<(YKStringA& str, long lVal);
YKStringW& operator<<(YKStringW& str, long lVal);
YKStringA& operator>>(YKStringA& str, long& lVal);
YKStringW& operator>>(YKStringW& str, long& lVal);

// Write or read unsigned long(DWORD) to or from a string.

YKStringA& operator<<(YKStringA& str, unsigned long ulVal);
YKStringW& operator<<(YKStringW& str, unsigned long ulVal);
YKStringA& operator>>(YKStringA& str, unsigned long& ulVal);
YKStringW& operator>>(YKStringW& str, unsigned long& ulVal);

// Write or read __int64 to or from a string.

YKStringA& operator<<(YKStringA& str, __int64 n64Val);
YKStringW& operator<<(YKStringW& str, __int64 n64Val);
YKStringA& operator>>(YKStringA& str, __int64& n64Val);
YKStringW& operator>>(YKStringW& str, __int64& n64Val);

// Write or read float to or from a string.

YKStringA& operator<<(YKStringA& str, float fVal);
YKStringW& operator<<(YKStringW& str, float fVal);
YKStringA& operator>>(YKStringA& str, float& fVal);
YKStringW& operator>>(YKStringW& str, float& fVal);

// Write or read double to or from a string.

YKStringA& operator<<(YKStringA& str, double fVal);
YKStringW& operator<<(YKStringW& str, double fVal);
YKStringA& operator>>(YKStringA& str, double& fVal);
YKStringW& operator>>(YKStringW& str, double& fVal);

// Write a single character.

YKStringA& operator<<(YKStringA& str, YKCHAR ch);
YKStringW& operator<<(YKStringW& str, YKWCHAR ch);

// Write or read a string value to or from another string.

YKStringA& operator<<(YKStringA& str, YKLPCSTR pStr);
YKStringW& operator<<(YKStringW& str, YKLPCWSTR pStr);
YKStringA& operator>>(YKStringA& str, YKStringA& strOut);
YKStringW& operator>>(YKStringW& str, YKStringW& strOut);

// Read string from strSrc to strDest until matching a character "chStop".

YKLPCSTR YKReadStringUntilChar(YKStringA& strDest, YKCHAR chStop, YKStringA& strSrc);
YKLPCWSTR YKReadStringUntilChar(YKStringW& strDest, YKWCHAR chStop, YKStringW& strSrc);

//////////////////////////////////////////////////////////////////////////
// Base class for operating file.
// You should not use this class directly.

class CYKArchive
{
public:
	// File open mode.
	enum EOpenMode
	{
		OM_INVALID = -1,
		OM_READONLY,			// Only read allowed.
		OM_WRITEONLY,			// Only write allowed.
		OM_APPEND,				// Append data to the end of file.
		OM_READPLUS,			// Read first, and then write.
		OM_WRITEPLUS,			// Write first, and then read.
		OM_APPENDPLUS,			// Append first, and then read.
		OM_END
	};

	// File seek origin type.
	enum ESeekMode
	{
		SM_INVALID = -1,
		SM_SEEK_BEGIN,			// From the beginning of file.
		SM_SEEK_CURRENT,		// From the current position of file.
		SM_SEEK_END,			// From the end of file.
		SM_END
	};

public:
	CYKArchive();
	explicit CYKArchive(YKLPCSTR szFileName, int nOpenMode);
	explicit CYKArchive(YKLPCWSTR szFileName, int nOpenMode);
	virtual ~CYKArchive();

public:
	// Open file.
	bool Open();

	// Close file, if this file is not closed, the destructor will close it.
	bool Close();

	// If this file opened or valid.
	bool IsValid() const;

	// Is reading state or writing state.
	// This is only valid when using OM_READONLY, OM_WRITEONLY or OM_APPEND open mode.
	bool IsLoading() const;

	// Rewind the current file position to the beginning.
	void Rewind();

	// Seek the current file position.
	int Seek(long lLength, int nOrigin);

	// How many bytes that the offset of the current file position from the beginning.
	long Tell();

	// Is end of this file(only for reading operation).
	bool IsEndOfFile() const;

	// Get file name.(mbcs)
	YKLPCSTR GetFileNameA() const;

	// Get file name.(unicode)
	YKLPCWSTR GetFileNameW() const;

	// Set file name.(mbcs)
	void SetFileNameA(YKLPCSTR szFileName);

	// Set file name.(unicode)
	void SetFileNameW(YKLPCWSTR szFileName);

	// Set file open mode.
	void SetOpenMode(int nOpenMode);

	// Get file open mode.
	int GetOpenMode() const;

	// Get file size in bytes.
	unsigned long GetFileSize() const;

protected:
	// Get the open mode string.(mbcs)
	virtual void SetOpenModeString(
		YKLPCSTR* ppOMRead, 
		YKLPCSTR* ppOMWrite, 
		YKLPCSTR* ppOMAppend,
		YKLPCSTR* ppOMReadPlus,
		YKLPCSTR* ppOMWritePlus,
		YKLPCSTR* ppPMAppendPlus
		) = 0;

	// Get the open mode string.(unicode)
	virtual void SetOpenModeString(
		YKLPCWSTR* ppOMRead, 
		YKLPCWSTR* ppOMWrite, 
		YKLPCWSTR* ppOMAppend,
		YKLPCWSTR* ppOMReadPlus,
		YKLPCWSTR* ppOMWritePlus,
		YKLPCWSTR* ppPMAppendPlus
		) = 0;

protected:
	FILE*         m_pFile;			// File pointer.
	YKStringA     m_szFileNameA;	// File name.(mbcs)
	YKStringW     m_szFileNameW;	// File name.(unicode)
	int           m_nOpenMode;		// Open mode.
	unsigned long m_dwFileSize;		// File size.

	bool m_bLoading;				// Reading or writing mode.
	bool m_bOpen;					// Is file opened.
};

//////////////////////////////////////////////////////////////////////////
// Bin archive.

class CYKBinArchive : public CYKArchive
{
public:
	CYKBinArchive();
	explicit CYKBinArchive(YKLPCSTR szFileName, int nOpenMode);
	explicit CYKBinArchive(YKLPCWSTR szFileName, int nOpenMode);
	virtual ~CYKBinArchive();

protected:
	// Get the open mode string.(mbcs)
	void SetOpenModeString(
		YKLPCSTR* ppOMRead, 
		YKLPCSTR* ppOMWrite, 
		YKLPCSTR* ppOMAppend,
		YKLPCSTR* ppOMReadPlus,
		YKLPCSTR* ppOMWritePlus,
		YKLPCSTR* ppOMAppendPlus
		);

	// Get the open mode string.(unicode)
	void SetOpenModeString(
		YKLPCWSTR* ppOMRead, 
		YKLPCWSTR* ppOMWrite, 
		YKLPCWSTR* ppOMAppend,
		YKLPCWSTR* ppOMReadPlus,
		YKLPCWSTR* ppOMWritePlus,
		YKLPCWSTR* ppOMAppendPlus
		);

public:
	// !CAUTION: you'd better to set certain structure alignment of defining
	// the stuctures that you want to use the following functions to write or
	// read, to avoid different size of structures when write or read files
	// on different platform or different compiler configuration.
	// e.g. set the structures to align by 1 byte:
	//      #pragma pack(push, 1)
	//      structures definition...
	//      #pragma pack(pop)

	// Write 1 data of type "T" to file.
	template <typename T> 
	CYKBinArchive& operator<<(const T& t);

	// Read 1 data of type "T" from file.
	template <typename T> 
	CYKBinArchive& operator>>(T& t);

	// Write array data.
	// Write data of type "T" from an array whose length is "dwCount" to file.
	template <typename T> 
	unsigned long WriteArray(const T* t, unsigned long dwCount);

	// Read data of type "T" into an array whose length is "dwCount" from file.
	template <typename T> 
	unsigned long ReadArray(T* t, unsigned long dwCount);

	// Write a string to file from the current file position.(mbcs)
	// (It starts from the current file pointer.)
	unsigned long WriteString(const YKStringA& str);

	// Write a string to file from the current file position.(unicode)
	// (It starts from the current file pointer.)
	unsigned long WriteString(const YKStringW& str);

	// Read a string from file from the current file position.(mbcs)
	unsigned long ReadString(YKStringA& str);

	// Read a string from file from the current file position.(unicode)
	unsigned long ReadString(YKStringW& str);

protected:
	static YKLPCSTR s_szOMReadA;
	static YKLPCSTR s_szOMWriteA;
	static YKLPCSTR s_szOMAppendA;
	static YKLPCSTR s_szOMReadPlusA;
	static YKLPCSTR s_szOMWritePlusA;
	static YKLPCSTR s_szOMAppendPlusA;

	static YKLPCWSTR s_szOMReadW;
	static YKLPCWSTR s_szOMWriteW;
	static YKLPCWSTR s_szOMAppendW;
	static YKLPCWSTR s_szOMReadPlusW;
	static YKLPCWSTR s_szOMWritePlusW;
	static YKLPCWSTR s_szOMAppendPlusW;

	static const unsigned long s_dwReadStrBufSize;
};

//////////////////////////////////////////////////////////////////////////
// Txt archive.

class CYKTxtArchive : public CYKArchive
{
public:
	CYKTxtArchive();
	explicit CYKTxtArchive(YKLPCSTR szFileName, int nOpenMode);
	explicit CYKTxtArchive(YKLPCWSTR szFileName, int nOpenMode);
	virtual ~CYKTxtArchive();

protected:
	// Get the open mode string.(mbcs)
	void SetOpenModeString(
		YKLPCSTR* ppOMRead, 
		YKLPCSTR* ppOMWrite, 
		YKLPCSTR* ppOMAppend,
		YKLPCSTR* ppOMReadPlus,
		YKLPCSTR* ppOMWritePlus,
		YKLPCSTR* ppOMAppendPlus
		);

	// Get the open mode string.(unicode)
	void SetOpenModeString(
		YKLPCWSTR* ppOMRead, 
		YKLPCWSTR* ppOMWrite, 
		YKLPCWSTR* ppOMAppend,
		YKLPCWSTR* ppOMReadPlus,
		YKLPCWSTR* ppOMWritePlus,
		YKLPCWSTR* ppOMAppendPlus
		);

public:
	// !CAUTION: remember to use "setlocale/_tsetlocale/_wsetlocale"(e.g. setlocale(LC_ALL, "chs");) 
	// before using the following functions at UNICODE mode on Windows platform,
	// or some characters like Chinese will not be written or read correctly.

	// Write a single character to file from the current file position.(mbcs)
	CYKTxtArchive& operator<<(YKCHAR ch);

	// Write a single character to file from the current file position.(unicode)
	CYKTxtArchive& operator<<(YKWCHAR ch);

	// Read a single character from file from the current file position.(mbcs)
	CYKTxtArchive& operator>>(YKCHAR& ch);

	// Read a single character from file from the current file position.(unicode)
	CYKTxtArchive& operator>>(YKWCHAR& ch);

	// Write a string to file from the current file position.(mbcs)
	bool WriteString(YKLPCSTR pStr);

	// Write a string to file from the current file position.(unicode)
	bool WriteString(YKLPCWSTR pStr);

	// Read a string whose length is "nLen" from file from the current file position.(mbcs)
	bool ReadString(YKStringA& str, int nLen);

	// Read a string whose length is "nLen" from file from the current file position.(unicode)
	bool ReadString(YKStringW& str, int nLen);

	// Read a line(not including the end of the '\n').(mbcs)
	// (it starts from the current file position.)
	void ReadLine(YKStringA& str);

	// Read a line(not including the end of the '\n').(unicode)
	// (it starts from the current file position.)
	void ReadLine(YKStringW& str);

	// Read whole text.(mbcs)
	// (It assume the file pointer is at origin, or it just read the rest of text.)
	bool ReadEntireText(YKStringA& str);

	// Read whole text.(unicode)
	// (It assume the file pointer is at origin, or it just read the rest of text.)
	bool ReadEntireText(YKStringW& str);

protected:
	static YKLPCSTR s_szOMReadA;
	static YKLPCSTR s_szOMWriteA;
	static YKLPCSTR s_szOMAppendA;
	static YKLPCSTR s_szOMReadPlusA;
	static YKLPCSTR s_szOMWritePlusA;
	static YKLPCSTR s_szOMAppendPlusA;

	static YKLPCWSTR s_szOMReadW;
	static YKLPCWSTR s_szOMWriteW;
	static YKLPCWSTR s_szOMAppendW;
	static YKLPCWSTR s_szOMReadPlusW;
	static YKLPCWSTR s_szOMWritePlusW;
	static YKLPCWSTR s_szOMAppendPlusW;

	static const unsigned long s_dwReadStrBufSize;
};

//////////////////////////////////////////////////////////////////////////
// Bin archive serializer usage base class.
// Your class should derived from this.

class CYKBinArchiveSerializer
{
public:
	CYKBinArchiveSerializer();
	explicit CYKBinArchiveSerializer(YKLPCSTR szFileName, int nOpenMode);
	explicit CYKBinArchiveSerializer(YKLPCWSTR szFileName, int nOpenMode);
	virtual ~CYKBinArchiveSerializer();

public:
	bool Go();

protected:
	virtual unsigned long Serialize(CYKBinArchive& ar);

public:
	enum EOpenMode
	{
		OM_INVALID   = CYKBinArchive::OM_INVALID,
		OM_READONLY  = CYKBinArchive::OM_READONLY,
		OM_WRITEONLY = CYKBinArchive::OM_WRITEONLY,
		OM_END       = CYKBinArchive::OM_END
	};

protected:
	YKStringA m_szFileNameA;			// File name.(mbcs)
	YKStringW m_szFileNameW;			// File name.(unicode)
	int       m_nOpenMode;				// Open mode.
};

//////////////////////////////////////////////////////////////////////////
// Txt archive serializer usage base class.
// Your class should derived from this.

class CYKTxtArchiveSerializer
{
public:
	CYKTxtArchiveSerializer();
	explicit CYKTxtArchiveSerializer(YKLPCSTR szFileName, int nOpenMode);
	explicit CYKTxtArchiveSerializer(YKLPCWSTR szFileName, int nOpenMode);
	virtual ~CYKTxtArchiveSerializer();

public:
	bool Go();

protected:
	virtual unsigned long Serialize(CYKTxtArchive& ar);

public:
	enum EOpenMode
	{
		OM_INVALID   = CYKTxtArchive::OM_INVALID,
		OM_READONLY  = CYKTxtArchive::OM_READONLY,
		OM_WRITEONLY = CYKTxtArchive::OM_WRITEONLY,
		OM_END       = CYKTxtArchive::OM_END
	};

protected:
	YKStringA m_szFileNameA;			// File name.(mbcs)
	YKStringW m_szFileNameW;			// File name.(unicode)
	int       m_nOpenMode;				// Open mode.
};

//////////////////////////////////////////////////////////////////////////
// The code below is not the user interface,
// just some template implementation of member functions of the prior classes.

template <typename T> 
CYKBinArchive& CYKBinArchive::operator<<(const T& t)
{
	ASSERT(NULL != m_pFile);

	if (!m_bOpen)
	{
		ASSERT(0);
		return *this;
	}

	m_dwFileSize += ((unsigned long)fwrite(&t, sizeof(T), 1, m_pFile) * sizeof(T));
	return *this;
}

template <typename T> 
CYKBinArchive& CYKBinArchive::operator>>(T& t)
{
	ASSERT(NULL != m_pFile);

	if (!m_bOpen)
	{
		ASSERT(0);
		return *this;
	}

	unsigned long dwSize = (unsigned long)fread(&t, sizeof(T), 1, m_pFile) * sizeof(T);
	ASSERT(0 != dwSize);

	return *this;
}

template <typename T> 
unsigned long CYKBinArchive::WriteArray(const T* t, unsigned long dwCount)
{
	ASSERT(NULL != m_pFile);

	if (!m_bOpen)
	{
		ASSERT(0);
		return 0;
	}

	unsigned long dwSize = (unsigned long)fwrite(t, sizeof(T), dwCount, m_pFile) * sizeof(T);
	m_dwFileSize += dwSize;

	return dwSize;
}

template <typename T> 
unsigned long CYKBinArchive::ReadArray(T* t, unsigned long dwCount)
{
	ASSERT(NULL != m_pFile);

	if (!m_bOpen)
	{
		ASSERT(0);
		return 0;
	}

	if (dwCount * sizeof(T) > m_dwFileSize)
	{
		ASSERT(0);
		return 0;
	}

	unsigned long dwSize = (unsigned long)fread(t, sizeof(T), dwCount, m_pFile) * sizeof(T);
	return dwSize;
}

#endif	// __YKARCHIVE_H__
