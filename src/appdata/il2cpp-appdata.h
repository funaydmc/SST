#pragma once

#include <cstdint>

#define DO_APP_FUNC(a, r, n, p) extern r (*n) p
namespace app {
#include "il2cpp-functions.h"
}
#undef DO_APP_FUNC