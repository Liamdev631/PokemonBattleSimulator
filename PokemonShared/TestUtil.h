#pragma once
#include <assert.h>

#ifdef _DEBUG
#define DebugAssert(expr, message) (void)(assert((expr) && message))
#else
#define DebugAssert(expr, message) ((void)0)
#endif