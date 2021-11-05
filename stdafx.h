#pragma once

#include "stdafx.h"
#include <string>
#include <locale>
#include <codecvt>
#include <assert.h>

#define KGLOG_PROCESS_ERROR(c) if (!(c)) { printf("KGLOG_PROCESS_ERROR(%s) at file: %s, line:%d, function:%s\n", #c, __FILE__, __LINE__, __FUNCTION__); goto Exit0;}
#define SAFE_CUT_OFF(array) array[sizeof(array) - 1] = 0;
#define SAFE_STR_CPY(ds, src) strncpy_s(ds, src, sizeof(ds)); SAFE_CUT_OFF(ds);

#define GBK_CODE_PAGE ".936"

inline std::wstring to_wide_string(const std::string & input)
{
	static std::wstring_convert<std::codecvt_utf8<wchar_t>> converter_utf8;
	static std::wstring_convert<std::codecvt_byname<wchar_t, char, mbstate_t>> 
		converter_gbk(new std::codecvt_byname < wchar_t, char, mbstate_t>(GBK_CODE_PAGE));
	std::wstring result;
	
	try {
		result = converter_utf8.from_bytes(input);
	}
 	catch (...)
 	{
 		result = converter_gbk.from_bytes(input);
 	}
	return result;
}

inline std::string to_byte_string(const std::wstring & input)
{
	static std::wstring_convert<std::codecvt_utf8<wchar_t>> converter_utf8;
	static std::wstring_convert<std::codecvt_byname<wchar_t, char, mbstate_t>> 
		converter_gbk(new std::codecvt_byname < wchar_t, char, mbstate_t>(GBK_CODE_PAGE));
	std::string result;

	try {
		result = converter_utf8.to_bytes(input);
	}
	catch (...)
	{
		result = converter_gbk.to_bytes(input);
	}
	return result;
}