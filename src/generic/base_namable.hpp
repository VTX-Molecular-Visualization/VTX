#ifndef __VTX_BASE_NAMABLE__
#define __VTX_BASE_NAMABLE__

#ifdef _MSC_VER
#pragma once
#endif

#include <string>

namespace VTX
{
	namespace Generic
	{
		class BaseNamable
		{
		  public:
			virtual std::string getName() const = 0;
		};
	} // namespace Generic
} // namespace VTX
#endif
