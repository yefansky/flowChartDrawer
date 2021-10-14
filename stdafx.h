#pragma once

#include "stdafx.h"
#define KGLOG_PROCESS_ERROR(c) if (!(c)) { printf("KGLOG_PROCESS_ERROR(%s) at file: %s, line:%d, function:%s", #c, __FILE__, __LINE__, __FUNCTION__); goto Exit0;}
#define SAFE_CUT_OFF(array) array[sizeof(array) - 1] = 0;
#define SAFE_STR_CPY(ds, src) strncpy(ds, src, sizeof(ds)); SAFE_CUT_OFF(ds);