#pragma once

#include <cstdint>

#if defined(_MSC_VER)

#define TPP_PACK_BEGIN __pragma(pack(push, 1))
#define TPP_PACK_END   __pragma(pack(pop))

#endif

namespace tpp
{
	template<bool B, class T, class F>
	struct conditional { typedef T type; };

	template<class T, class F>
	struct conditional<false, T, F> { typedef F type; };

	template< bool B, class T, class F>
	using conditional_t = typename conditional<B, T, F>::type;

	inline void Assert(bool condition)
	{
		if (!condition)
		{
			int* i = nullptr;
			*i = 0;
		}
	}
};