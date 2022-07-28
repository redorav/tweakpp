#pragma once

#include <cstdint>

#if defined(_MSC_VER)

#define TPP_PACK_BEGIN __pragma(pack(push, 1))
#define TPP_PACK_END   __pragma(pack(pop))

#endif