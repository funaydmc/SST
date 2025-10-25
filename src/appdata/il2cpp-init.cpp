#include <pch.h>

#define DO_APP_FUNC(a, r, n, p) r (*n) p
namespace app {
#include "il2cpp-functions.h"
}
#undef DO_APP_FUNC


void init_il2cpp()
{
	uintptr_t baseAddress = (uintptr_t)GetModuleHandle("GameAssembly.dll");

	#define DO_APP_FUNC(a, r, n, p) n = (r (*) p)(baseAddress + a)
	#include "il2cpp-functions.h"
	#undef DO_APP_FUNC
}