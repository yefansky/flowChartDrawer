#pragma once

#include "stdafx.h"
#include <string>
#include <locale>
#include <codecvt>
#include <assert.h>

#define KGLOG_PROCESS_ERROR(c) if (!(c)) { printf("KGLOG_PROCESS_ERROR(%s) at file: %s, line:%d, function:%s\n", #c, __FILE__, __LINE__, __FUNCTION__); goto Exit0;}
#define SAFE_CUT_OFF(array) array[sizeof(array) - 1] = 0;
#define SAFE_STR_CPY(ds, src) strncpy_s(ds, src, sizeof(ds)); SAFE_CUT_OFF(ds);

inline std::wstring to_wide_string(const std::string & input)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
	return converter.from_bytes(input);
}
// convert wstring to string 
inline std::string to_byte_string(const std::wstring & input)
{
	//std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
	return converter.to_bytes(input);
}