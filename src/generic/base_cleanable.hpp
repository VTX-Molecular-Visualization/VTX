#ifndef __VTX_BASE_CLEANABLE__
#define __VTX_BASE_CLEANABLE__

#ifdef _MSC_VER
#pragma once
#endif

#include "define.hpp"

namespace VTX
{
	namespace Generic
	{
		class BaseCleanable
		{
		  public:
			virtual void clean() = 0;
		};
	} // namespace Generic
} // namespace VTX
#endif
